#include "evrythng.h"

#include "CuTest.h"

#include <stdio.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#define MQTT_BROKER_TCP_URL "tcp://localhost:1883"
#define MQTT_BROKER_SSL_URL "ssl://localhost:8883"
#define THNG_1 "thng1"
#define PRODUCT_1 "product1"
#define PROPERTY_1 "prop1"
#define PROPERTY_2 "prop2"
#define ACTION_1 "action1"

#define PROPERTY_VALUE_JSON "[{\"value\": 500}]"
#define PROPERTIES_VALUE_JSON "[{\"key\": \"prop1\", \"value\": 500}, {\"key\": \"prop2\", \"value\": 100}]"

#define ACTION_JSON "[{\"type\": \"action1\"}]"
#define ACTIONS_JSON "[{\"type\": \"action1\"}, {\"type\": \"action2\"}]"

#define LOCATION_JSON  "[{\"position\": { \"type\": \"Point\", \"coordinates\": [-17.3, 36] }}]"


sem_t pub_sem;
sem_t sub_sem;

void test_init_handle_ok(CuTest* tc)
{
    evrythng_handle_t h;
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_init_handle(&h));
    evrythng_destroy_handle(h);
}

void test_init_handle_fail(CuTest* tc)
{
    CuAssertIntEquals(tc, EVRYTHNG_BAD_ARGS, evrythng_init_handle(0));
}

void test_destroy_handle(CuTest* tc)
{
    evrythng_handle_t h;
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_init_handle(&h));
    evrythng_destroy_handle(h);
}

void test_set_url_ok(CuTest* tc)
{
    evrythng_handle_t h;
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_init_handle(&h));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_set_url(h, "tcp://localhost:666"));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_set_url(h, "ssl://localhost:666"));
    evrythng_destroy_handle(h);
}

void test_set_url_fail(CuTest* tc)
{
    evrythng_handle_t h;
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_init_handle(&h));
    CuAssertIntEquals(tc, EVRYTHNG_BAD_ARGS, evrythng_set_url(h, 0));
    CuAssertIntEquals(tc, EVRYTHNG_BAD_URL, evrythng_set_url(h, "ttt://localhost:666"));
    evrythng_destroy_handle(h);
}

void test_set_key_ok(CuTest* tc)
{
    evrythng_handle_t h;
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_init_handle(&h));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_set_key(h, "123456789"));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_set_key(h, "123456789123456789"));
    evrythng_destroy_handle(h);
}

void test_set_client_id_ok(CuTest* tc)
{
    evrythng_handle_t h;
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_init_handle(&h));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_set_client_id(h, "123456789"));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_set_client_id(h, "123456789123456789"));
    evrythng_destroy_handle(h);
}

void test_set_ca_ok(CuTest* tc)
{
    evrythng_handle_t h;
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_init_handle(&h));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_set_certificate(h, "123", 3));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_set_certificate(h, "1", 1));
    evrythng_destroy_handle(h);
}

void test_set_ca_fail(CuTest* tc)
{
    evrythng_handle_t h;
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_init_handle(&h));
    CuAssertIntEquals(tc, EVRYTHNG_BAD_ARGS, evrythng_set_certificate(h, "123", 0));
    CuAssertIntEquals(tc, EVRYTHNG_BAD_ARGS, evrythng_set_certificate(h, 0, 1));
    evrythng_destroy_handle(h);
}

void test_set_qos_ok(CuTest* tc)
{
    evrythng_handle_t h;
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_init_handle(&h));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_set_qos(h, 0));
    evrythng_destroy_handle(h);
}

void test_set_qos_fail(CuTest* tc)
{
    evrythng_handle_t h;
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_init_handle(&h));
    CuAssertIntEquals(tc, EVRYTHNG_BAD_ARGS, evrythng_set_qos(h, 8));
    evrythng_destroy_handle(h);
}

void log_callback(evrythng_log_level_t level, const char* fmt, va_list vl) {}

void test_set_callback_ok(CuTest* tc)
{
    evrythng_handle_t h;
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_init_handle(&h));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_set_log_callback(h, log_callback));
    evrythng_destroy_handle(h);
}

void test_set_callback_fail(CuTest* tc)
{
    evrythng_handle_t h;
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_init_handle(&h));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_set_log_callback(h, 0));
    evrythng_destroy_handle(h);
}

static void common_tcp_init_handle(evrythng_handle_t* h)
{
    int rc = evrythng_init_handle(h);
    rc = evrythng_set_url(*h, MQTT_BROKER_TCP_URL);
    rc = evrythng_set_log_callback(*h, default_log_callback);
}

void test_tcp_connect_ok1(CuTest* tc)
{
    evrythng_handle_t h;
    common_tcp_init_handle(&h);
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_disconnect(h));
    evrythng_destroy_handle(h);
}

void test_tcp_connect_ok2(CuTest* tc)
{
    evrythng_handle_t h;
    common_tcp_init_handle(&h);
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_connect(h));
    evrythng_disconnect(h);
    evrythng_destroy_handle(h);
}

void test_tcp_connect_ok3(CuTest* tc)
{
    evrythng_handle_t h1;
    evrythng_handle_t h2;
    evrythng_handle_t h3;
    common_tcp_init_handle(&h1);
    common_tcp_init_handle(&h2);
    common_tcp_init_handle(&h3);
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_connect(h1));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_connect(h2));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_connect(h3));
    evrythng_disconnect(h1);
    evrythng_disconnect(h2);
    evrythng_disconnect(h3);
    evrythng_destroy_handle(h1);
    evrythng_destroy_handle(h2);
    evrythng_destroy_handle(h3);
}

static void test_pub_callback()
{
    printf("%s: message published\n", __func__);
    sem_post(&pub_sem);
}

static void test_sub_callback(const char* str_json)
{
    printf("%s: %s\n", __func__, str_json);
    sem_post(&sub_sem);
}

#define START_PUBSUB \
    evrythng_handle_t h1;\
    evrythng_handle_t h2;\
    common_tcp_init_handle(&h1);\
    common_tcp_init_handle(&h2);\
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_connect(h1));\
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_connect(h2));

#define END_PUBSUB \
    struct timespec ts;\
    clock_gettime(CLOCK_REALTIME, &ts);\
    ts.tv_sec += 5;\
    CuAssertIntEquals(tc, 0, sem_timedwait(&pub_sem, &ts));\
    clock_gettime(CLOCK_REALTIME, &ts);\
    ts.tv_sec += 5;\
    CuAssertIntEquals(tc, 0, sem_timedwait(&sub_sem, &ts));\
    evrythng_disconnect(h1);\
    evrythng_disconnect(h2);\
    evrythng_destroy_handle(h1);\
    evrythng_destroy_handle(h2);\

void test_pubsub_thng_prop(CuTest* tc)
{
    START_PUBSUB
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_subscribe_thng_property(h2, THNG_1, PROPERTY_1, test_sub_callback));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_publish_thng_property(h1, THNG_1, PROPERTY_1, PROPERTY_VALUE_JSON, test_pub_callback));
    END_PUBSUB
}

void test_pubsuball_thng_prop(CuTest* tc)
{
    START_PUBSUB
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_subscribe_thng_property(h2, THNG_1, PROPERTY_1, test_sub_callback));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_publish_thng_property(h1, THNG_1, PROPERTY_1, PROPERTIES_VALUE_JSON, test_pub_callback));
    END_PUBSUB
}

void test_pubsub_thng_action(CuTest* tc)
{
    START_PUBSUB
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_subscribe_thng_action(h2, THNG_1, ACTION_1, test_sub_callback));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_publish_thng_action(h1, THNG_1, ACTION_1, ACTION_JSON, test_pub_callback));
    END_PUBSUB
}

void test_pubsuball_thng_actions(CuTest* tc)
{
    START_PUBSUB
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_subscribe_thng_actions(h2, THNG_1, test_sub_callback));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_publish_thng_actions(h1, THNG_1, ACTIONS_JSON, test_pub_callback));
    END_PUBSUB
}

void test_pubsub_thng_location(CuTest* tc)
{
    START_PUBSUB
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_subscribe_thng_location(h2, THNG_1, test_sub_callback));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_publish_thng_location(h1, THNG_1, LOCATION_JSON, test_pub_callback));
    END_PUBSUB
}

void test_pubsub_prod_prop(CuTest* tc)
{
    START_PUBSUB
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_subscribe_product_property(h2, PRODUCT_1, PROPERTY_1, test_sub_callback));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_publish_product_property(h1, PRODUCT_1, PROPERTY_1, PROPERTY_VALUE_JSON, test_pub_callback));
    END_PUBSUB
}

void test_pubsuball_prod_prop(CuTest* tc)
{
    START_PUBSUB
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_subscribe_product_properties(h2, PRODUCT_1, test_sub_callback));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_publish_product_properties(h1, PRODUCT_1, PROPERTIES_VALUE_JSON, test_pub_callback));
    END_PUBSUB
}

void test_pubsub_prod_action(CuTest* tc)
{
    START_PUBSUB
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_subscribe_product_action(h2, PRODUCT_1, ACTION_1, test_sub_callback));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_publish_product_action(h1, PRODUCT_1, ACTION_1, ACTION_JSON, test_pub_callback));
    END_PUBSUB
}

void test_pubsuball_prod_actions(CuTest* tc)
{
    START_PUBSUB
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_subscribe_product_actions(h2, PRODUCT_1, test_sub_callback));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_publish_product_actions(h1, PRODUCT_1, ACTIONS_JSON, test_pub_callback));
    END_PUBSUB
}

void test_pubsub_action(CuTest* tc)
{
    START_PUBSUB
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_subscribe_action(h2, ACTION_1, test_sub_callback));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_publish_action(h1, ACTION_1, ACTION_JSON, test_pub_callback));
    END_PUBSUB
}

void test_pubsuball_actions(CuTest* tc)
{
    START_PUBSUB
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_subscribe_actions(h2, test_sub_callback));
    CuAssertIntEquals(tc, EVRYTHNG_SUCCESS, evrythng_publish_actions(h1, ACTIONS_JSON, test_pub_callback));
    END_PUBSUB
}

CuSuite* CuGetSuite(void)
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_init_handle_ok);
	SUITE_ADD_TEST(suite, test_init_handle_fail);
	SUITE_ADD_TEST(suite, test_destroy_handle);
	SUITE_ADD_TEST(suite, test_set_url_ok);
	SUITE_ADD_TEST(suite, test_set_url_fail);
	SUITE_ADD_TEST(suite, test_set_key_ok);
	SUITE_ADD_TEST(suite, test_set_client_id_ok);
	SUITE_ADD_TEST(suite, test_set_ca_ok);
	SUITE_ADD_TEST(suite, test_set_ca_fail);
	SUITE_ADD_TEST(suite, test_set_qos_ok);
	SUITE_ADD_TEST(suite, test_set_qos_fail);
	SUITE_ADD_TEST(suite, test_set_callback_ok);
	SUITE_ADD_TEST(suite, test_set_callback_fail);

	SUITE_ADD_TEST(suite, test_tcp_connect_ok1);
	SUITE_ADD_TEST(suite, test_tcp_connect_ok2);
	SUITE_ADD_TEST(suite, test_tcp_connect_ok3);

	SUITE_ADD_TEST(suite, test_pubsub_thng_prop);
	SUITE_ADD_TEST(suite, test_pubsuball_thng_prop);

	SUITE_ADD_TEST(suite, test_pubsub_thng_action);
	SUITE_ADD_TEST(suite, test_pubsuball_thng_actions);

#if 1
	SUITE_ADD_TEST(suite, test_pubsub_thng_location);

	SUITE_ADD_TEST(suite, test_pubsub_prod_prop);
	SUITE_ADD_TEST(suite, test_pubsuball_prod_prop);

	SUITE_ADD_TEST(suite, test_pubsub_prod_action);
	SUITE_ADD_TEST(suite, test_pubsuball_prod_actions);

	SUITE_ADD_TEST(suite, test_pubsub_action);
	SUITE_ADD_TEST(suite, test_pubsuball_actions);
#endif

	return suite;
}


void RunAllTests(void)
{
	CuString *output = CuStringNew();

    CuSuite* suite = CuGetSuite();

	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);
    
    CuSuiteDelete(suite);
    CuStringDelete(output);
}

int main(void)
{
    sem_init(&pub_sem, 0, 0);
    sem_init(&sub_sem, 0, 0);

	RunAllTests();
}
