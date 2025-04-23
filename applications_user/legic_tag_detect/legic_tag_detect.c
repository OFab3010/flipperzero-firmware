/*
 * Legic Tag Detector App for Flipper Zero
 * Test
 * Anleitung:
 * 1. Klone das Flipper Zero Firmware-Repository.
 * 2. Lege im Verzeichnis firmware/applications-user/ folgendes an:
 *      - Ordner: legic_tag_detect
 *      - Datei: legic_tag_detect.c   (diese Quelldatei)
 *      - Datei: CMakeLists.txt      (siehe unten)
 * 3. Ergänze in firmware/applications-user/CMakeLists.txt:
 *      add_application(NAME legic_tag_detect)
 * 4. Baue die Firmware oder nur deine App via GitHub Actions/CLI.
 * 5. Lade das erzeugte .fap-Artefakt (build/applications-user/legic_tag_detect.fap)
 *    in der Flipper Zero Mobile App hoch.
 *
 * Hinweis:
 * Ersetze LEGIC_PRIME_ATQA/SAK und LEGIC_ADVANT_ATQA/SAK mit deinen Tag-Parametern.
 */
#include <furi.h>
#include <gui/gui.h>
#include <nfc/nfc.h>
#include <nfc/nfc_target.h>

// Legic Prime Parameter
#define LEGIC_PRIME_ATQA 0x0044  // Beispiel-ATQA für Prime
#define LEGIC_PRIME_SAK  0x20    // Beispiel-SAK für Prime

// Legic Advant Parameter
#define LEGIC_ADVANT_ATQA 0x0004 // Beispiel-ATQA für Advant
#define LEGIC_ADVANT_SAK  0x08   // Beispiel-SAK für Advant

int32_t legic_tag_detect(void* p) {
    FURI_LOG_I("legic_tag_detect", "Starte Legic Prime und Advant Erkennung");

    Gui* gui = furi_record_open("gui");
    Nfc* nfc = furi_record_open("nfc");

    while(true) {
        NfcTarget target;
        // Poll auf ISO14443A (1 Slot, Timeout 500ms)
        if(nfc_initiator_poll(nfc, &target, 1, 500) == NFC_OK && target.type == NFC_TYPE_A) {
            uint16_t atqa = target.info.type_a.atqa;
            uint8_t sak = target.info.type_a.sak;
            char buf[80];
            const char* type_str = NULL;
            if(atqa == LEGIC_PRIME_ATQA && sak == LEGIC_PRIME_SAK) {
                type_str = "Legic Prime";
            } else if(atqa == LEGIC_ADVANT_ATQA && sak == LEGIC_ADVANT_SAK) {
                type_str = "Legic Advant";
            }
            if(type_str) {
                snprintf(buf, sizeof(buf), "%s erkannt!\nUID: %02X:%02X:%02X:%02X", 
                    type_str,
                    target.info.type_a.uid[0],
                    target.info.type_a.uid[1],
                    target.info.type_a.uid[2],
                    target.info.type_a.uid[3]
                );
            } else {
                snprintf(buf, sizeof(buf), "Kein Legic Prime/Advant Tag in Reichweite");
            }
            gui_show_message(gui, buf, GuiAlignmentCenter);
        } else {
            // Kein Tag oder Timeout
            gui_show_message(gui,
                "Kein Tag in Reichweite",
                GuiAlignmentCenter
            );
        }
        furi_delay_ms(1000);
    }

    furi_record_close("nfc");
    furi_record_close("gui");
    return 0;
}

// Registrierung der App
AppCEntry(legic_tag_detect);

/*
 * CMakeLists.txt (optional in legic_tag_detect-Ordner)
 * Alternativ genügt ein Eintrag in firmware/applications-user/CMakeLists.txt:
 * add_application(NAME legic_tag_detect)
 *
 * Für ein eigenständiges CMakeLists.txt könntest du folgendes nutzen:
 *
 * add_library(legic_tag_detect STATIC      
 *     legic_tag_detect.c                   
 * )                                        
 * target_link_libraries(legic_tag_detect   
 *     furi gui nfc                           
 * )                                        
 * add_app(legic_tag_detect)
 */
