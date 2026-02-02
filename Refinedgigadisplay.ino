#include "Arduino_H7_Video.h"
#include "Arduino_GigaDisplayTouch.h"
#include "lvgl.h"

Arduino_H7_Video          Display(800, 480, GigaDisplayShield); /* Arduino_H7_Video Display(1024, 768, USBCVideo); */
Arduino_GigaDisplayTouch  TouchDetector;

void Create_UI()
{
  // Create a container for the left column
    lv_obj_t * left_col = lv_obj_create(lv_scr_act());
    lv_obj_set_size(left_col, 240, 480);  // Set the size of the left column container
    lv_obj_align(left_col, LV_ALIGN_LEFT_MID, 0, 0);  // Align to the left mid

    // Title
    lv_obj_t * title = lv_label_create(left_col);
    lv_label_set_text(title, "SEPTALPRO CONTROLLER");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    // Connection Status Box
    lv_obj_t * con_status_box = lv_obj_create(left_col);
    lv_obj_set_size(con_status_box, 220, 100);
    lv_obj_align(con_status_box, LV_ALIGN_TOP_MID, 0, 70);
    lv_obj_set_style_bg_color(con_status_box, lv_color_make(0,0,255), 0);
    lv_obj_set_style_border_width(con_status_box, 0, 0);

    // Connection Status Text
    lv_obj_t * con_status_label = lv_label_create(con_status_box);
    lv_label_set_text(con_status_label, "Connection Status\nRobot: CONNECTED\nSimulation: CONNECTED");
    lv_obj_center(con_status_label);

     // Tool Switching Status
    lv_obj_t * tool_switching_box = lv_obj_create(left_col);
    lv_obj_set_size(tool_switching_box, 220, 50);  // Size of the tool switching status box
    lv_obj_align(tool_switching_box, LV_ALIGN_TOP_MID, 0, 200);
    lv_obj_set_style_bg_color(tool_switching_box, lv_color_make(0,0,255), 0);
    lv_obj_set_style_border_width(tool_switching_box, 0, 0);

    lv_obj_t * tool_switching_label = lv_label_create(tool_switching_box);
    lv_label_set_text(tool_switching_label, "Tool Switching in Progress....");
    lv_obj_center(tool_switching_label);

    // Control Mode Box (empty for now, add content as needed)
    lv_obj_t * control_mode_box = lv_obj_create(left_col);
    lv_obj_set_size(control_mode_box, 220, 150);  // Size of the control mode box
    lv_obj_align(control_mode_box, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_bg_color(control_mode_box, lv_color_make(0,0,255), 0);
    lv_obj_set_style_border_width(control_mode_box, 0, 0);

    // control mode box text
    lv_obj_t * conmodelabel = lv_label_create(control_mode_box);
    lv_label_set_text(conmodelabel, "Control Mode:");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 0);

    // Create a container for the right column
    lv_obj_t * right_col = lv_obj_create(lv_scr_act());
    lv_obj_set_size(right_col, 300, 100);  // Set the size of the right column container
    lv_obj_align(right_col, LV_ALIGN_RIGHT_MID, -50, -150);  // Align to the right mid

    // Title
    lv_obj_t * title2 = lv_label_create(right_col);
    lv_label_set_text(title2, "Tool Status");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_align(title2, LV_ALIGN_TOP_MID, 0, 0);
}


static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked");
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled");
    }
}

void lv_example_btn_1(void)
{
    lv_obj_t * label;

    lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn1, LV_ALIGN_RIGHT_MID, -150, -40);
    lv_obj_add_flag(btn1, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_height(btn1, LV_SIZE_CONTENT);

    label = lv_label_create(btn1);
    lv_label_set_text(label, "Guidewire");
    lv_obj_center(label);

    lv_obj_t * btn2 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn2, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn2, LV_ALIGN_RIGHT_MID, -150, 40);
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_height(btn2, LV_SIZE_CONTENT);

    label = lv_label_create(btn2);
    lv_label_set_text(label, "Needle");
    lv_obj_center(label);

    lv_obj_t * btn3 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn3, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn3, LV_ALIGN_RIGHT_MID, -150, 120);
    lv_obj_add_flag(btn3, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_height(btn3, LV_SIZE_CONTENT);

    label = lv_label_create(btn3);
    lv_label_set_text(label, "Dilator-Sheath");
    lv_obj_center(label);
}


void setup()
{
  Display.begin();
  TouchDetector.begin();
  Serial.begin(115200);

  lv_obj_t * screen = lv_obj_create(lv_scr_act());
  lv_obj_set_size(screen, Display.width(), Display.height());

  static lv_coord_t col_dsc[] = { 500, LV_GRID_TEMPLATE_LAST};
  static lv_coord_t row_dsc[] = { 400, LV_GRID_TEMPLATE_LAST};

  lv_obj_t * grid = lv_obj_create(lv_scr_act());
  lv_obj_set_grid_dsc_array(grid, col_dsc, row_dsc);
  lv_obj_set_size(grid, Display.width(), Display.height());
  lv_obj_center(grid);
  lv_example_btn_1();
  Create_UI();
}

void loop()
{
  lv_timer_handler();
}


