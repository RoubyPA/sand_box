/*  file : main.c
 *  Tuto : https://gtk.developpez.com/cours/gtk2/?page=page_4
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gtk/gtk.h>

#define SIZE_X 500
#define SIZE_Y 150

void
on_destroy(GtkWidget *w, gpointer data)
{
  /* Break gtk main loop */
  gtk_main_quit();

  printf("Bye ! <3\n");
}

int
main(int argc, char **argv)
{
  GtkWidget *pWindow;
  GtkWidget *pLabel;
  GtkWidget *pButton;
  gchar     *utf8_buf;

  /* Init gkt env */
  gtk_init(&argc, &argv);

  /* Create window */
  pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

/******************************************************************************
 *  Window                                                                    *
 ******************************************************************************/

  /* Window size */
  gtk_window_set_default_size(GTK_WINDOW(pWindow), SIZE_X, SIZE_Y);
  /* Window title */
  gtk_window_set_title(GTK_WINDOW(pWindow), "GTK <3");
  /* Set window position */
  gtk_window_set_position(GTK_WINDOW(pWindow), GTK_WIN_POS_CENTER);


/******************************************************************************
 *  Label                                                                     *
 ******************************************************************************/

  /* Set gchar in UTF8 */
//  utf8_buf = g_locale_to_utf8("HelloWorld !", -1, NULL, NULL, NULL);
  /* Create label */
//  pLabel  = gtk_label_new(utf8_buf);
  /* Free UTF8 Buffer */
//  g_free(utf8_buf);
  /* Align text */
//  gtk_label_set_justify(GTK_LABEL(pLabel), GTK_JUSTIFY_LEFT);
  /* Add label in window */
//  gtk_container_add(GTK_CONTAINER(pWindow), pLabel);


/******************************************************************************
 *  Button                                                                    *
 ******************************************************************************/
  pButton = gtk_button_new_with_label("Quit");
  //pButton = gtk_button_new_with_mnemonic("_Exit");
  //pButton = gtk_button_new_from_stock(GTK_STOCK_QUIT);
  /* Add label in window */
  gtk_container_add(GTK_CONTAINER(pWindow), pButton);


/******************************************************************************
 *  Display                                                                   *
 ******************************************************************************/
  /* Display window */
  //gtk_widget_show(pWindow);
  gtk_widget_show_all(pWindow);

  /* Block window resize */
  //gtk_window_set_resizable(GTK_WINDOW(pWindow), FALSE);

  /*  Window position
   *  void gtk_window_move(GtkWindow *window, gint x, gint y);
   */

  /*  Window resize
   *  void gtk_window_resize(GtkWindow *window, gint width, gint height);
   */

  /* Hide title of window */
  //gtk_window_set_decorated(GTK_WINDOW(pWindow), FALSE);

  /* Add event listenner on window destroy */
  g_signal_connect( G_OBJECT(pWindow),
                    "destroy",
                    G_CALLBACK(on_destroy),
                    NULL);

  /* Add event listenner on window destroy */
  g_signal_connect( G_OBJECT(pButton),
                    "clicked",
                    G_CALLBACK(on_destroy),
                    NULL);

  /* Event loop */
  gtk_main();

  return EXIT_SUCCESS;
}
