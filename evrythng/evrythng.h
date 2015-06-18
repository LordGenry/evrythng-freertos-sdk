/*! \mainpage API documentation
 *
 * \link evrythng.h \endlink
 *
 * \section examp_sec Examples
 *
 * \link evrythng-demo.c \endlink
 *
 */

/**
 *
 * @file   evrythng.h
 * @brief  Evrythng API
 *
 * \example evrythng-demo.c
 *
 **/


#ifndef _EVRYTHNG_H
#define _EVRYTHNG_H


#include <stdlib.h>
#include <stdarg.h>


typedef enum _evrythng_return_t 
{
    EVRYTHNG_SUBSCRIPTION_ERROR  = -9,
    EVRYTHNG_PUBLISH_ERROR       = -8,
    EVRYTHNG_NOT_CONNECTED       = -7,
    EVRYTHNG_CONNECTION_FAILED   = -6,
    EVRYTHNG_BAD_URL             = -5,
    EVRYTHNG_CERT_REQUIRED_ERROR = -4,
    EVRYTHNG_MEMORY_ERROR        = -3,
    EVRYTHNG_BAD_ARGS            = -2,
    EVRYTHNG_FAILURE             = -1,
    EVRYTHNG_SUCCESS             =  0,
} evrythng_return_t;


typedef enum 
{
    EVRYTHNG_LOG_ERROR   = 0, 
    EVRYTHNG_LOG_WARNING = 1, 
    EVRYTHNG_LOG_DEBUG   = 2, 
} evrythng_log_level_t;


/** @brief Log callback prototype.
 */
typedef void (*evrythng_log_callback)(evrythng_log_level_t level, const char* fmt, va_list vl); 


/** @brief Default callback using stdout to print messages.
 */
void default_log_callback(evrythng_log_level_t level, const char* fmt, va_list vl);


/** @brief Pointer to internal context used by all the functions.
 */
typedef struct evrythng_ctx_t* evrythng_handle_t;


/** @brief Callback prototype used for subscribe functions,
 *  	   which is called on message arrival from the Evrythng
 *  	   cloud.
 */
typedef void sub_callback(const char* str_json);


/** @brief Callback prototype used for publish functions,
 *  	   which is called on message delivered event from the
 *  	   Evrythng cloud.
 */
typedef void pub_callback(void);


/** @brief Initialize context.
 *
 * Use this function to initialize context which contains Evrythng client configuration
 * used for connecting to the Evrythng cloud. Use evrythng_destroy_handle to deinit it.
 *
 * @param[in] handle A pointer to context handle.
 *
 * @return    \b EVRYTHNG_BAD_ARGS     if handle is a null pointer \n
 *            \b EVRYTHNG_MEMORY_ERROR if an error occured while allocating memory \n
 *            \b EVRYTHNG_SUCCESS      on success \n
 */
evrythng_return_t evrythng_init_handle(evrythng_handle_t* handle);


/** @brief Destroy context.
 *
 * Use this function to deinitialize context.
 *
 * @param[in] handle A context handle.
 *
 * @return void
 */
void evrythng_destroy_handle(evrythng_handle_t handle);


/** @brief Set URL to connect to.
 *
 * Use this function to set URL to internal context, tcp://<ip>:<port> for 
 * unencrypted connection and ssl://<ip>:<port> for SSL.
 *
 * @param[in] handle A pointer to context handle.
 * @param[in] url A pointer to url.
 *
 * @return    \b EVRYTHNG_BAD_ARGS     if handle or url is null pointer or url does not start
 *                                     with a tcp or ssl \n
 *            \b EVRYTHNG_MEMORY_ERROR if an error occured while allocating memory \n
 *            \b EVRYTHNG_SUCCESS      on success \n
 */
evrythng_return_t evrythng_set_url(evrythng_handle_t handle, const char* url);


/** @brief Set API key to use for connecting to the cloud.
 *
 * Use this function to set API key to internal context 
 *
 * @param[in] handle A pointer to context handle.
 * @param[in] key A pointer to key.
 *
 * @return    \b EVRYTHNG_BAD_ARGS     if handle or key is null pointer \n
 *            \b EVRYTHNG_MEMORY_ERROR if an error occured while allocating memory \n
 *            \b EVRYTHNG_SUCCESS      on success \n
 */
evrythng_return_t evrythng_set_key(evrythng_handle_t handle, const char* key);


/** @brief Set client id to use for connecting to the cloud.
 *
 * Use this function to set client id to internal context.
 * If client id was not setup a random 10 digits key will be generated.
 *
 * @param[in] handle A pointer to context handle.
 * @param[in] url A pointer to client id.
 *
 * @return    \b EVRYTHNG_BAD_ARGS     if handle or key is null pointer \n
 *            \b EVRYTHNG_MEMORY_ERROR if an error occured while allocating memory \n
 *            \b EVRYTHNG_SUCCESS      on success \n
 */
evrythng_return_t evrythng_set_client_id(evrythng_handle_t handle, const char* client_id);


/** @brief Set QoS to use for this connection.
 *
 * Use this function to set QoS.
 * If QoS was not setup a default value of 2 will be used.
 * See MQTT docs for more details.
 *
 * @param[in] handle A pointer to context handle.
 * @param[in] qos A Qos value.
 *
 * @return    \b EVRYTHNG_BAD_ARGS     if handle is a null pointer or QoS is > 2 or < 0 \n
 *            \b EVRYTHNG_SUCCESS      on success \n
 */
evrythng_return_t evrythng_set_qos(evrythng_handle_t handle, int qos);


/** @brief Set certificate to use for encrypted connection.
 *
 * Use this function to set certificate to internal context.
 *
 * @param[in] handle A pointer to context handle.
 * @param[in] cert A pointer to certificate.
 * @param[in] size Size in bytes of a certificate.
 *
 * @return    \b EVRYTHNG_BAD_ARGS     if handle or key is a null pointer or size <= 0 \n
 *            \b EVRYTHNG_MEMORY_ERROR if an error occured while allocating memory \n
 *            \b EVRYTHNG_SUCCESS      on success \n
 */
evrythng_return_t evrythng_set_certificate(evrythng_handle_t handle, const char* cert, size_t size);


/** @brief Set log callback
 *
 * Use this function to set log callback to internal context 
 * otherwise a null pointer will be setup.  One can use default 
 * internal log function with output to stdout.
 * To disable logging set 0 value or empty function.
 *
 * @param[in] handle A pointer to context handle.
 * @param[in] callback A pointer to log callback.
 *
 * @return    \b EVRYTHNG_BAD_ARGS     if handle or callback is a null pointer \n
 *            \b EVRYTHNG_SUCCESS      on success \n
 */
evrythng_return_t evrythng_set_log_callback(evrythng_handle_t handle, evrythng_log_callback callback);


/** @brief Connect to Evrythng cloud.
 *
 * Use this function to connect to the Evrythng cloud.
 *
 * @param[in] handle    A handle to context which contains Evrythng client configuration
 *                      used for connecting to the Evrythng cloud.
 *
 * @return    \b EVRYTHNG_BAD_ARGS if handle is a null pointer \n
 *            \b EVRYTHNG_CERT_REQUIRED_ERROR if SSL url is setup but certificate was not provided\n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_FAILURE if mqtt client creation/setup error occured \n
 *            \b EVRYTHNG_CONNECTION_FAILED if could not establish connection to the cloud \n
 *            \b EVRYTHNG_SUCCESS      on success \n
 */
evrythng_return_t evrythng_connect(evrythng_handle_t handle);


/** @brief Disconnect from Evrythng cloud.
 *
 * Use this function to disconnect from Evrythng cloud.
 *
 * @param[in] handle    A handle to context which contains Evrythng client configuration
 *                      used for connecting to the Evrythng cloud.
 *
 * @return    \b EVRYTHNG_BAD_ARGS if handle is a null pointer \n
 *            \b EVRYTHNG_FAILURE if mqtt client creation/setup error occured \n
 *            \b EVRYTHNG_SUCCESS      on success \n
 */
evrythng_return_t evrythng_disconnect(evrythng_handle_t handle);


/** @brief Publish a single property to a given thing.
 *
 * This function attempts to publish a single property to a given thing.
 *
 * @param[in] handle        A context handle.
 * @param[in] thng_id A     A thing ID.
 * @param[in] property_name The name of the property.
 * @param[in] property_json A JSON string which contains property value. 
 * @param[in] callback      A pointer to a publish callback function.
 * 
 * @return    \b EVRYTHNG_BAD_ARGS if one the arguments is a null pointer or a too long string \n
 *            \b EVRYTHNG_PUBLISH_ERROR if an error occured trying to publish a message \n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_NOT_CONNECTED if internal context is not in connected state \n
 *            \b EVRYTHNG_SUCCESS on success \n
 */
evrythng_return_t evrythng_publish_thng_property(
        evrythng_handle_t handle, 
        const char* thng_id, 
        const char* property_name, 
        const char* property_json, 
        pub_callback *callback);


/** @brief Subscribe to a single property of the thing.
 *
 * This function attempts to subscribe to a single property of the thing.
 *  
 * @param[in] handle        A context handle.
 * @param[in] thng_id       A thing ID.
 * @param[in] property_name The name of the property. 
 * @param[in] callback      A pointer to a subscribe callback function. 
 *
 * @return    \b EVRYTHNG_BAD_ARGS if one the arguments is a null pointer or a too long string \n
 *            \b EVRYTHNG_SUBSCRIPTION_ERROR if an error occured trying to subscribe to a topic \n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_NOT_CONNECTED if internal context is not in connected state \n
 *            \b EVRYTHNG_SUCCESS on success \n
 */
evrythng_return_t evrythng_subscribe_thng_property(
        evrythng_handle_t handle, 
        const char* thng_id, 
        const char* property_name, 
        sub_callback *callback);


/** @brief Subscribe to all properties of the thing.
 * 
 * This function subscribes to all properties of the thing.
 *
 * @param[in] handle   A context handle.
 * @param[in] thng_id  A thing ID. 
 * @param[in] callback A pointer to a subscribe callback function. 
 *
 * @return    \b EVRYTHNG_BAD_ARGS if one the arguments is a null pointer or a too long string \n
 *            \b EVRYTHNG_SUBSCRIPTION_ERROR if an error occured trying to subscribe to a topic \n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_NOT_CONNECTED if internal context is not in connected state \n
 *            \b EVRYTHNG_SUCCESS on success \n
 */
evrythng_return_t evrythng_subscribe_thng_properties(
        evrythng_handle_t handle, 
        const char* thng_id, 
        sub_callback *callback);


/** @brief Publish a few properties to a given thing.
 *
 * This function attempts to publish a few properties to a given thing.
 *
 * @param[in] handle          A context handle.
 * @param[in] thng_id         A thing ID.
 * @param[in] properties_json A JSON string which contains properties values. 
 * @param[in] callback        A pointer to a publish callback function. 
 *
 * @return    \b EVRYTHNG_BAD_ARGS if one the arguments is a null pointer or a too long string \n
 *            \b EVRYTHNG_PUBLISH_ERROR if an error occured trying to publish a message \n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_NOT_CONNECTED if internal context is not in connected state \n
 *            \b EVRYTHNG_SUCCESS on success \n
 */
evrythng_return_t evrythng_publish_thng_properties(
        evrythng_handle_t handle, 
        const char* thng_id, 
        const char* properties_json, 
        pub_callback *callback);


/** @brief Subscribe to a single action of the thing.
 *
 * This function attempts to subscribe to a single action of the thing.
 *
 * @param[in] handle      A context handle.
 * @param[in] thng_id     A thing ID.
 * @param[in] action_name The name of an action. 
 * @param[in] callback    A pointer to a subscribe callback function. 
 *
 * @return    \b EVRYTHNG_BAD_ARGS if one the arguments is a null pointer or a too long string \n
 *            \b EVRYTHNG_SUBSCRIPTION_ERROR if an error occured trying to subscribe to a topic \n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_NOT_CONNECTED if internal context is not in connected state \n
 *            \b EVRYTHNG_SUCCESS on success \n
 */
evrythng_return_t evrythng_subscribe_thng_action(
        evrythng_handle_t handle, 
        const char* thng_id, 
        const char* action_name, 
        sub_callback *callback);


/** @brief Subscribe to all actions of the thing.
 *
 * This function attempts to subscribe to all actions of the thing.
 *
 * @param[in] handle   A context handle.
 * @param[in] thng_id  A thing ID. 
 * @param[in] callback A pointer to a subscribe callback function. 
 *
 * @return    \b EVRYTHNG_BAD_ARGS if one the arguments is a null pointer or a too long string \n
 *            \b EVRYTHNG_SUBSCRIPTION_ERROR if an error occured trying to subscribe to a topic \n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_NOT_CONNECTED if internal context is not in connected state \n
 *            \b EVRYTHNG_SUCCESS on success \n
 */
evrythng_return_t evrythng_subscribe_thng_actions(
        evrythng_handle_t handle, 
        const char* thng_id, 
        sub_callback *callback);


/** @brief Publish a single action to a given thing. 
 *
 * This function attempts to publish a single action to a given thing. 
 *
 * @param[in] handle      A context handle.
 * @param[in] thng_id     A thing ID.
 * @param[in] action_name The name of an action.
 * @param[in] action_json A JSON string which contains an action. 
 * @param[in] callback    A pointer to a publish callback function. 
 *
 * @return    \b EVRYTHNG_BAD_ARGS if one the arguments is a null pointer or a too long string \n
 *            \b EVRYTHNG_PUBLISH_ERROR if an error occured trying to publish a message \n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_NOT_CONNECTED if internal context is not in connected state \n
 *            \b EVRYTHNG_SUCCESS on success \n
 */
evrythng_return_t evrythng_publish_thng_action(
        evrythng_handle_t handle, 
        const char* thng_id, 
        const char* action_name, 
        const char* action_json, 
        pub_callback *callback);


/** @brief Publish a few actions to a given thing.
 *
 * This function attempts to publish a few actions to a given thing.
 *
 * @param[in] handle       A context handle.
 * @param[in] thng_id      A thing ID.
 * @param[in] actions_json A JSON string which contains actions. 
 * @param[in] callback     A pointer to a publish callback function. 
 *
 * @return    \b EVRYTHNG_BAD_ARGS if one the arguments is a null pointer or a too long string \n
 *            \b EVRYTHNG_PUBLISH_ERROR if an error occured trying to publish a message \n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_NOT_CONNECTED if internal context is not in connected state \n
 *            \b EVRYTHNG_SUCCESS on success \n
 */
evrythng_return_t evrythng_publish_thng_actions(
        evrythng_handle_t handle, 
        const char* thng_id, 
        const char* actions_json, 
        pub_callback *callback);


/** @brief Subscribe to a location of the thing.
 *
 * This function attempts to subscribe to a location of the thing.
 *
 * @param[in] handle   A context handle.
 * @param[in] thng_id  A thing ID. 
 * @param[in] callback A pointer to a subscribe callback function. 
 *
 * @return    \b EVRYTHNG_BAD_ARGS if one the arguments is a null pointer or a too long string \n
 *            \b EVRYTHNG_SUBSCRIPTION_ERROR if an error occured trying to subscribe to a topic \n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_NOT_CONNECTED if internal context is not in connected state \n
 *            \b EVRYTHNG_SUCCESS on success \n
 */
evrythng_return_t evrythng_subscribe_thng_location(
        evrythng_handle_t handle, 
        const char* thng_id, 
        sub_callback *callback);


/** @brief Publish a location to a given thing.
 *
 * This function attempts to publish a location to a given thing.
 *
 * @param[in] handle        A context handle.
 * @param[in] thng_id       A thing ID.
 * @param[in] location_json A JSON string which contains location. 
 * @param[in] callback      A pointer to a publish callback function. 
 *
 * @return    \b EVRYTHNG_BAD_ARGS if one the arguments is a null pointer or a too long string \n
 *            \b EVRYTHNG_PUBLISH_ERROR if an error occured trying to publish a message \n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_NOT_CONNECTED if internal context is not in connected state \n
 *            \b EVRYTHNG_SUCCESS on success \n
 */
evrythng_return_t evrythng_publish_thng_location(
        evrythng_handle_t handle, 
        const char* thng_id, 
        const char* location_json, 
        pub_callback *callback);


/** @brief Subscribe to a single property of the product.
 *
 * This function attempts to subscribe to a single property of the product.
 *
 * @param[in] handle        A context handle.
 * @param[in] product_id    A product ID.
 * @param[in] property_name The name of the property. 
 * @param[in] callback      A pointer to a subscribe callback function. 
 *
 * @return    \b EVRYTHNG_BAD_ARGS if one the arguments is a null pointer or a too long string \n
 *            \b EVRYTHNG_SUBSCRIPTION_ERROR if an error occured trying to subscribe to a topic \n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_NOT_CONNECTED if internal context is not in connected state \n
 *            \b EVRYTHNG_SUCCESS on success \n
 */
evrythng_return_t evrythng_subscribe_product_property(
        evrythng_handle_t handle, 
        const char* product_id, 
        const char* property_name, 
        sub_callback *callback);


/** @brief Subscribe to all properties of the product.
 *
 * This function attempts to subscribe to all properties of the product.
 *
 * @param[in] handle      A context handle.
 * @param[in] product_id A product ID. 
 * @param[in] callback    A pointer to a subscribe callback function. 
 *
 * @return    \b EVRYTHNG_BAD_ARGS if one the arguments is a null pointer or a too long string \n
 *            \b EVRYTHNG_SUBSCRIPTION_ERROR if an error occured trying to subscribe to a topic \n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_NOT_CONNECTED if internal context is not in connected state \n
 *            \b EVRYTHNG_SUCCESS on success \n
 */
evrythng_return_t evrythng_subscribe_product_properties(
        evrythng_handle_t handle, 
        const char* product_id, 
        sub_callback *callback);


/** @brief Publish a single property to a given product.
 *
 * This function attempts to publish a single property to a given product.
 *
 * @param[in] handle        A context handle.
 * @param[in] product_id    A product ID.
 * @param[in] property_name The name of the property.
 * @param[in] property_json A JSON string which contains property value. 
 * @param[in] callback      A pointer to a publish callback function. 
 *
 * @return    \b EVRYTHNG_BAD_ARGS if one the arguments is a null pointer or a too long string \n
 *            \b EVRYTHNG_PUBLISH_ERROR if an error occured trying to publish a message \n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_NOT_CONNECTED if internal context is not in connected state \n
 *            \b EVRYTHNG_SUCCESS on success \n
 */
evrythng_return_t evrythng_publish_product_property(
        evrythng_handle_t handle, 
        const char* product_id, 
        const char* property_name, 
        const char* property_json, 
        pub_callback *callback);


/** @brief Publish a few properties to a given product.
 *
 * This function attempts to publish a few properties to a given product.
 *
 * @param[in] handle          A context handle.
 * @param[in] product_id      A product ID.
 * @param[in] properties_json A JSON string which contains properties values. 
 * @param[in] callback        A pointer to a publish callback function. 
 *
 * @return    \b EVRYTHNG_BAD_ARGS if one the arguments is a null pointer or a too long string \n
 *            \b EVRYTHNG_PUBLISH_ERROR if an error occured trying to publish a message \n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_NOT_CONNECTED if internal context is not in connected state \n
 *            \b EVRYTHNG_SUCCESS on success \n
 */
evrythng_return_t evrythng_publish_product_properties(
        evrythng_handle_t handle, 
        const char* product_id, 
        const char* properties_json, 
        pub_callback *callback);


/** @brief Subscribe to a single action of the product.
 *
 * This function attempts to subscribe to a single action of the product.
 *
 * @param[in] handle      A context handle.
 * @param[in] product_id  A product ID.
 * @param[in] action_name The name of an action. 
 * @param[in] callback    A pointer to a subscribe callback function. 
 *
 * @return    \b EVRYTHNG_BAD_ARGS if one the arguments is a null pointer or a too long string \n
 *            \b EVRYTHNG_SUBSCRIPTION_ERROR if an error occured trying to subscribe to a topic \n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_NOT_CONNECTED if internal context is not in connected state \n
 *            \b EVRYTHNG_SUCCESS on success \n
 */
evrythng_return_t evrythng_subscribe_product_action(
        evrythng_handle_t handle, 
        const char* product_id, 
        const char* action_name, 
        sub_callback *callback);


/** @brief Subscribe to all actions of the product.
 *
 * This function attempts to subscribe to all actions of the product.
 *
 * @param[in] handle      A context handle.
 * @param[in] product_id  A product ID. 
 * @param[in] callback    A pointer to a subscribe callback function. 
 *
 * @return    \b EVRYTHNG_BAD_ARGS if one the arguments is a null pointer or a too long string \n
 *            \b EVRYTHNG_SUBSCRIPTION_ERROR if an error occured trying to subscribe to a topic \n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_NOT_CONNECTED if internal context is not in connected state \n
 *            \b EVRYTHNG_SUCCESS on success \n
 */
evrythng_return_t evrythng_subscribe_product_actions(
        evrythng_handle_t handle, 
        const char* product_id, 
        sub_callback *callback);


/** @brief Publish a single action to a given product.
 *
 * This function attempts to publish a single action to a given product. 
 *
 * @param[in] handle      A context handle.
 * @param[in] product_id  A product ID.
 * @param[in] action_name The name of an action.
 * @param[in] action_json A JSON string which contains an action. 
 * @param[in] callback    A pointer to a publish callback function. 
 *
 * @return    \b EVRYTHNG_BAD_ARGS if one the arguments is a null pointer or a too long string \n
 *            \b EVRYTHNG_PUBLISH_ERROR if an error occured trying to publish a message \n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_NOT_CONNECTED if internal context is not in connected state \n
 *            \b EVRYTHNG_SUCCESS on success \n
 */
evrythng_return_t evrythng_publish_product_action(
        evrythng_handle_t handle, 
        const char* product_id, 
        const char* action_name, 
        const char* action_json, 
        pub_callback *callback);


/** @brief Publish a few actions to a given product.
 *
 * This function attempts to publish a few actions to a given product.
 *
 * @param[in] handle       A context handle.
 * @param[in] product_id   A product ID.
 * @param[in] actions_json A JSON string which contains actions. 
 * @param[in] callback     A pointer to a publish callback function. 
 *
 * @return    \b EVRYTHNG_BAD_ARGS if one the arguments is a null pointer or a too long string \n
 *            \b EVRYTHNG_PUBLISH_ERROR if an error occured trying to publish a message \n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_NOT_CONNECTED if internal context is not in connected state \n
 *            \b EVRYTHNG_SUCCESS on success \n
 */
evrythng_return_t evrythng_publish_product_actions(
        evrythng_handle_t handle, 
        const char* product_id, 
        const char* actions_json, 
        pub_callback *callback);


/** @brief Subscribe to a single action.
 *
 * This function attempts to subscribe to a single action.
 *
 * @param[in] handle      A context handle.
 * @param[in] action_name The name of an action. 
 * @param[in] callback    A pointer to a subscribe callback function. 
 *
 * @return    \b EVRYTHNG_BAD_ARGS if one the arguments is a null pointer or a too long string \n
 *            \b EVRYTHNG_SUBSCRIPTION_ERROR if an error occured trying to subscribe to a topic \n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_NOT_CONNECTED if internal context is not in connected state \n
 *            \b EVRYTHNG_SUCCESS on success \n
 */
evrythng_return_t evrythng_subscribe_action(
        evrythng_handle_t handle, 
        const char* action_name, 
        sub_callback *callback);


/** @brief Subscribe to all actions.
 *
 * This function attempts to subscribe to all actions.
 *  
 * @param[in] handle   A context handle.
 * @param[in] callback A pointer to a subscribe callback function. 
 *
 * @return    \b EVRYTHNG_BAD_ARGS if one the arguments is a null pointer or a too long string \n
 *            \b EVRYTHNG_SUBSCRIPTION_ERROR if an error occured trying to subscribe to a topic \n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_NOT_CONNECTED if internal context is not in connected state \n
 *            \b EVRYTHNG_SUCCESS on success \n
 */
evrythng_return_t evrythng_subscribe_actions(
        evrythng_handle_t handle, 
        sub_callback *callback);


/** @brief Publish a single action.
 *
 * This function attempts to publish a single action.
 *
 * @param[in] handle      A context handle.
 * @param[in] action_name The name of an action.
 * @param[in] action_json A JSON string which contains an action. 
 * @param[in] callback    A pointer to a publish callback function. 
 *
 * @return    \b EVRYTHNG_BAD_ARGS if one the arguments is a null pointer or a too long string \n
 *            \b EVRYTHNG_PUBLISH_ERROR if an error occured trying to publish a message \n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_NOT_CONNECTED if internal context is not in connected state \n
 *            \b EVRYTHNG_SUCCESS on success \n
 */
evrythng_return_t evrythng_publish_action(
        evrythng_handle_t handle, 
        const char* action_name, 
        const char* action_json, 
        pub_callback *callback);


/** @brief Publish a few actions.
 *
 * This function attempts to publish a few actions.
 *
 * @param[in] handle       A context handle.
 * @param[in] actions_json A JSON string which contains actions. 
 * @param[in] callback     A pointer to a publish callback function. 
 *
 * @return    \b EVRYTHNG_BAD_ARGS if one the arguments is a null pointer or a too long string \n
 *            \b EVRYTHNG_PUBLISH_ERROR if an error occured trying to publish a message \n
 *            \b EVRYTHNG_MEMORY_ERROR if memory allocation error occured \n
 *            \b EVRYTHNG_NOT_CONNECTED if internal context is not in connected state \n
 *            \b EVRYTHNG_SUCCESS on success \n
 */
evrythng_return_t evrythng_publish_actions(
        evrythng_handle_t handle, 
        const char* actions_json, 
        pub_callback *callback);


#endif //_EVRYTHNG_H
