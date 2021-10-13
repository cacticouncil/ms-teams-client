#include <glib.h>
#include <libsoup/soup.h>

int main(){
    //glib Hello World
    GString* my_string = g_string_new("This Hello world is %d chars long\n");
    g_print(my_string->str, my_string->len);
    g_string_free(my_string, TRUE);

    //libsoup test
    SoupSession *session = soup_session_new ();
    //SoupMessageHeaders *response_headers;
    
    SoupMessage *msg = soup_message_new (SOUP_METHOD_GET, "https://upload.wikimedia.org/wikipedia/commons/5/5f/BBB-Bunny.png");
    
    //GInputStream *stream;
    GError *error = NULL;
    GInputStream *stream = soup_session_send(session,msg,NULL,&error);

    if (error) {
        g_printerr ("Failed to download: %s\n", error->message);
        g_error_free (error);
        g_input_stream_close(stream,NULL,&error);
        g_object_unref (msg);
        g_object_unref (session);
        return 1;
    }

    //response_headers = msg->response_headers;
    //const char *content_type;
    const char *content_type = soup_message_headers_get_content_type (msg->response_headers,NULL);

    // content_type = "image/png"
    // bytes contains the raw data that can be used elsewhere
    GBytes *bytes = g_input_stream_read_bytes(stream,8192,NULL,NULL);
    g_input_stream_close(stream,NULL,NULL);
    g_print ("Downloaded %zu bytes of type %s\n",
             g_bytes_get_size (bytes), content_type);
    delete[] content_type;

    g_bytes_unref (bytes);
    g_object_unref (msg);
    g_object_unref (session);

    return 0;
}