#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <gio/gio.h>

// Default values
gchar *bar_position = "top";
gint bar_size = 24;

// File monitor related variables
GFileMonitor *config_monitor = NULL;
GFileMonitor *menu_monitor = NULL;

void read_config();

// Callback function when the config file is changed
static void on_config_changed(GFileMonitor *monitor, GFile *file, GFile *other_file, GFileMonitorEvent event_type, gpointer user_data) {
    if (event_type == G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT) {
        g_printf("Config file changed, reloading...\n");
        read_config();
    }
}

// Callback function when the menu.py file is changed
static void on_menu_changed(GFileMonitor *monitor, GFile *file, GFile *other_file, GFileMonitorEvent event_type, gpointer user_data) {
    if (event_type == G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT) {
        g_printf("Menu script changed.\n");
    }
}

void read_config() {
    gchar *config_path = g_strdup_printf("%s/.config/xmenu/conf", g_get_home_dir());
    gchar *content = NULL;

    if (g_file_get_contents(config_path, &content, NULL, NULL)) {
        gchar **lines = g_strsplit(content, "\n", -1);
        for (int i = 0; lines[i] != NULL; i++) {
            gchar **line_parts = g_strsplit(lines[i], "=", 2);
            if (g_strv_length(line_parts) == 2) {
                gchar *key = g_strstrip(line_parts[0]);
                gchar *value = g_strstrip(line_parts[1]);

                if (g_strcmp0(key, "bar-position") == 0) {
                    bar_position = g_strdup(value);
                } else if (g_strcmp0(key, "bar-size") == 0) {
                    bar_size = g_ascii_strtoll(value, NULL, 10);
                }

                g_strfreev(line_parts);
            }
        }
        g_strfreev(lines);
        g_free(content);
    }

    g_free(config_path);
}

static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
        gchar *command = g_strdup_printf("python3 %s/.config/xmenu/menu.py", g_get_home_dir());
        system(command);
        g_free(command);
        return TRUE; // Stop propagation
    }
    return FALSE; // Continue propagation
}

static void activate(GtkApplication* app, gpointer user_data) {
    GtkWidget* w = gtk_application_window_new(app);

    if (g_strcmp0(bar_position, "top") == 0) {
        gtk_window_set_default_size(GTK_WINDOW(w), 1920, 1080 - bar_size);
        gtk_window_move(GTK_WINDOW(w), 0, bar_size);
    } else if (g_strcmp0(bar_position, "bottom") == 0) {
        gtk_window_set_default_size(GTK_WINDOW(w), 1920, 1080 - bar_size);
        gtk_window_move(GTK_WINDOW(w), 0, 0);
    } else if (g_strcmp0(bar_position, "left") == 0) {
        gtk_window_set_default_size(GTK_WINDOW(w), 1920 - bar_size, 1080);
        gtk_window_move(GTK_WINDOW(w), bar_size, 0);
    } else if (g_strcmp0(bar_position, "right") == 0) {
        gtk_window_set_default_size(GTK_WINDOW(w), 1920 - bar_size, 1080);
        gtk_window_move(GTK_WINDOW(w), 0, 0);
    }

    // Additional configurations
    gtk_window_set_decorated(GTK_WINDOW(w), FALSE);
    gtk_window_stick(GTK_WINDOW(w));
    gtk_window_set_type_hint(GTK_WINDOW(w), GDK_WINDOW_TYPE_HINT_DESKTOP);

    // Make the window transparent
    gtk_widget_set_app_paintable(w, TRUE);
    gtk_widget_set_visual(w, gdk_screen_get_rgba_visual(gdk_screen_get_default()));

    // Connect the button press event
    g_signal_connect(w, "button-press-event", G_CALLBACK(on_button_press), NULL);

    // Show all window content
    gtk_widget_show_all(w);
}

int main(int argc, char **argv) {
    read_config();

    // Setup file monitoring
    gchar *config_path = g_strdup_printf("%s/.config/xmenu/conf", g_get_home_dir());
    GFile *config_file = g_file_new_for_path(config_path);
    config_monitor = g_file_monitor_file(config_file, G_FILE_MONITOR_NONE, NULL, NULL);
    g_signal_connect(config_monitor, "changed", G_CALLBACK(on_config_changed), NULL);

    gchar *menu_path = g_strdup_printf("%s/.config/xmenu/menu.py", g_get_home_dir());
    GFile *menu_file = g_file_new_for_path(menu_path);
    menu_monitor = g_file_monitor_file(menu_file, G_FILE_MONITOR_NONE, NULL, NULL);
    g_signal_connect(menu_monitor, "changed", G_CALLBACK(on_menu_changed), NULL);

    GtkApplication* app = gtk_application_new("org.example.Xmenu", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    g_object_unref(config_monitor);
    g_object_unref(menu_monitor);

    return status;
}










// BASE
// #include <gtk/gtk.h>
// #include <glib.h>
// #include <glib/gprintf.h>
//
// // Default values
// gchar *bar_position = "top";
// gint bar_size = 24;
//
// void read_config() {
//     gchar *config_path = g_strdup_printf("%s/.config/xmenu/conf", g_get_home_dir());
//     gchar *content = NULL;
//
//     if (g_file_get_contents(config_path, &content, NULL, NULL)) {
//         gchar **lines = g_strsplit(content, "\n", -1);
//         for (int i = 0; lines[i] != NULL; i++) {
//             gchar **line_parts = g_strsplit(lines[i], "=", 2);
//             if (g_strv_length(line_parts) == 2) {
//                 gchar *key = g_strstrip(line_parts[0]);
//                 gchar *value = g_strstrip(line_parts[1]);
//
//                 if (g_strcmp0(key, "bar-position") == 0) {
//                     bar_position = g_strdup(value);
//                 } else if (g_strcmp0(key, "bar-size") == 0) {
//                     bar_size = g_ascii_strtoll(value, NULL, 10);
//                 }
//
//                 g_strfreev(line_parts);
//             }
//         }
//         g_strfreev(lines);
//         g_free(content);
//     }
//
//     g_free(config_path);
// }
//
// static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
//     if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
//         gchar *command = g_strdup_printf("python3 %s/.config/xmenu/menu.py", g_get_home_dir());
//         system(command);
//         g_free(command);
//         return TRUE; // Stop propagation
//     }
//     return FALSE; // Continue propagation
// }
//
// static void activate(GtkApplication* app, gpointer user_data) {
//     GtkWidget* w = gtk_application_window_new(app);
//
//     if (g_strcmp0(bar_position, "top") == 0) {
//         gtk_window_set_default_size(GTK_WINDOW(w), 1920, 1080 - bar_size);
//         gtk_window_move(GTK_WINDOW(w), 0, bar_size);
//     } else if (g_strcmp0(bar_position, "bottom") == 0) {
//         gtk_window_set_default_size(GTK_WINDOW(w), 1920, 1080 - bar_size);
//         gtk_window_move(GTK_WINDOW(w), 0, 0);
//     } else if (g_strcmp0(bar_position, "left") == 0) {
//         gtk_window_set_default_size(GTK_WINDOW(w), 1920 - bar_size, 1080);
//         gtk_window_move(GTK_WINDOW(w), bar_size, 0);
//     } else if (g_strcmp0(bar_position, "right") == 0) {
//         gtk_window_set_default_size(GTK_WINDOW(w), 1920 - bar_size, 1080);
//         gtk_window_move(GTK_WINDOW(w), 0, 0);
//     }
//
//     // Additional configurations
//     gtk_window_set_decorated(GTK_WINDOW(w), FALSE);
//     gtk_window_stick(GTK_WINDOW(w));
//     gtk_window_set_type_hint(GTK_WINDOW(w), GDK_WINDOW_TYPE_HINT_DESKTOP);
//
//     // Make the window transparent
//     gtk_widget_set_app_paintable(w, TRUE);
//     gtk_widget_set_visual(w, gdk_screen_get_rgba_visual(gdk_screen_get_default()));
//
//     // Connect the button press event
//     g_signal_connect(w, "button-press-event", G_CALLBACK(on_button_press), NULL);
//
//     // Show all window content
//     gtk_widget_show_all(w);
// }
//
// int main(int argc, char **argv) {
//     read_config();
//
//     GtkApplication* app = gtk_application_new("org.example.Xmenu", G_APPLICATION_DEFAULT_FLAGS);
//     g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
//     int status = g_application_run(G_APPLICATION(app), argc, argv);
//     g_object_unref(app);
//
//     return status;
// }



