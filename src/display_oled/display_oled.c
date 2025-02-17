#include "display_oled.h"
#include "src/ssd1306/ssd1306.h"
#include "src/ssd1306/ssd1306_font.h"
#include <string.h>

void show_text(char *text[], int num_lines)
{
    // Buffer para o display
    static uint8_t ssd[ssd1306_buffer_length];

    // Área de renderização (toda a tela)
    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    // Limpa a tela (reseta o display)
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    // Imprime cada linha do texto na tela
    for (int i = 0; i < num_lines; i++)
    {
        ssd1306_draw_string(ssd, 0, i * 8, text[i]); // Cada linha é separada por 8 pixels (altura do caractere)
    }

    // Renderiza o conteúdo na tela
    render_on_display(ssd, &frame_area);
}