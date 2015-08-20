/*
 * (c) Copyright 2012 EVRYTHNG Ltd London / Zurich
 * www.evrythng.com
 */

/**
 * The demo application for FreeRTOS which demonstrates how to
 * work with MQTT client. It connects to the Evrythng cloud and
 * sends publish messages every five seconds.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <time.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "evrythng.h"

#define log(_fmt_, ...) printf(_fmt_"\n\r", ##__VA_ARGS__)

typedef struct _cmd_opt {
    int sub;
    int pub;
    char* url;
    char* thng;
    char* key;
    char* prop;
    char* cafile;
    evrythng_handle_t evt_handle;
} cmd_opts; 


/** @brief This is a callback function which is called on the
 *  	   property receiving. It prints the received JSON
 *  	   string.
 */
void print_property_callback(const char* str_json, size_t len)
{
    char msg[len+1]; snprintf(msg, sizeof msg, "%s", str_json);
    log("Received message: %s", msg);
}


/** @brief This is queue trace send hook which is used by
 *  	   FreeRTOS.
 */
void vMainQueueSendPassed(void)
{
    /* This is just an example implementation of the "queue send" trace hook. */
}


/** @brief This is application idle hook which is used by
 *  	   FreeRTOS.
 */
void vApplicationIdleHook(void)
{
    sleep(1);
}


void log_callback(evrythng_log_level_t level, const char* fmt, va_list vl)
{
    char msg[512];

    unsigned n = vsnprintf(msg, sizeof msg, fmt, vl);
    if (n >= sizeof msg)
        msg[sizeof msg - 1] = '\0';

    switch (level)
    {
        case EVRYTHNG_LOG_ERROR:
            printf("ERROR: ");
            break;
        case EVRYTHNG_LOG_WARNING:
            printf("WARNING: ");
            break;
        default:
        case EVRYTHNG_LOG_DEBUG:
            printf("DEBUG: ");
            break;
    }
    printf("%s\n", msg);
}


void conlost_callback(evrythng_handle_t h)
{
    log("connection lost, exiting...");
}


#include <signal.h>
void _sleep(int sec)
{
    sigset_t sigmask, oldsigmask;
    sigfillset(&sigmask);
    pthread_sigmask(SIG_SETMASK, &sigmask, &oldsigmask);
    sleep(sec);
    pthread_sigmask(SIG_SETMASK, &oldsigmask, 0);
}


static void evrythng_task(void* pvParameters)
{
    cmd_opts* opts = (cmd_opts*)pvParameters;

    evrythng_init_handle(&opts->evt_handle);
    evrythng_set_log_callback(opts->evt_handle, log_callback);
    evrythng_set_conlost_callback(opts->evt_handle, conlost_callback);
    evrythng_set_url(opts->evt_handle, opts->url);
    evrythng_set_key(opts->evt_handle, opts->key);
    evrythng_set_certificate(opts->evt_handle, opts->cafile, opts->cafile ? strlen(opts->cafile) : 0);


    log("Connecting to %s", opts->url);
    while(evrythng_connect(opts->evt_handle) != EVRYTHNG_SUCCESS) {
        log("Retrying");
        sleep(2);
    }
    log("Evrythng client Connected");
    
    if (opts->sub) 
    {
        log("Subscribing to property %s", opts->prop);
        evrythng_subscribe_thng_property(opts->evt_handle, opts->thng, opts->prop, print_property_callback);
        while(1) sleep(1);
    } 
    else 
    {
        while(1) 
        {
            int value = rand() % 100;
            char msg[128];
            sprintf(msg, "[{\"value\": %d}]", value);
            log("Publishing value %d to property %s", value, opts->prop);
            evrythng_publish_thng_property(opts->evt_handle, opts->thng, opts->prop, msg, NULL);
            _sleep(2);
        }
    }
}


void print_usage() {
    log("Usage: evrtthng_demo -s|-p -u URL -t THNG_ID -k KEY -n PROPERTY_NAME [-v VALUE] [-c CA_FILE]");
    log("\t-s, --sub\tsubscribe to a property");
    log("\t-p, --pub\tpublish a property");
    log("\t-u, --url\tconnect using url (tcp://<url>+<port> for tcp and ssl://<url>+<port> for ssl connection");
    log("\t-t, --thng\tthing id");
    log("\t-k, --key\tauthentication key");
    log("\t-n, --prop\tproperty name to subscribe or publish");
    log("\t-c, --cafile\tcertificate file path is using ssl connection");
}


int main(int argc, char *argv[])
{
    cmd_opts opts = { 0 };
    struct option long_options[] = {
        {"sub",     no_argument,        0, 's' },
        {"pub",     no_argument,        0, 'p' },
        {"url",     required_argument,  0, 'u' },
        {"thng",    required_argument,  0, 't' },
        {"key",     required_argument,  0, 'k' },
        {"prop",    required_argument,  0, 'n' },
        {"cafile",  required_argument,  0, 'c' },
        {"help",    no_argument,        0, 'h' },
        {0, 0, 0, 0}
    };

    int long_index =0, opt;
    while ((opt = getopt_long(argc, argv,"spu:t:k:n:v:c:h", 
                    long_options, &long_index )) != -1) {
        switch (opt) {
            case 's' : opts.sub = 1;
                       break;
            case 'p' : opts.pub = 1;
                       break;
            case 'u' : opts.url = optarg; 
                       break;
            case 't' : opts.thng = optarg;
                       break;
            case 'k' : opts.key = optarg;
                       break;
            case 'n' : opts.prop = optarg;
                       break;
            case 'c' : opts.cafile = optarg;
                       break;
            case 'h' : 
            default:
                       print_usage(); 
                       exit(EXIT_FAILURE);
        }
    }

    if (opts.sub && opts.pub) {
        log("use --sub or --pub option, not both");
        exit(EXIT_FAILURE);
    }

    if (!opts.sub && !opts.pub) {
        log("use --sub or --pub option");
        exit(EXIT_FAILURE);
    }

    if (!opts.url || !opts.key || !opts.thng || !opts.prop) {
        print_usage();
        exit(EXIT_FAILURE);
    }

    xTaskCreate(evrythng_task, "evrythng_task", 1024, (void*)&opts, 0, NULL);

    vTaskStartScheduler();

    return EXIT_SUCCESS;
}
