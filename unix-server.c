#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <json-c/json.h>
#include <sys/stat.h>  // Для chmod
#include <maxminddb.h> // Для работы с libmaxminddb

// #include "./lib/maxminddb.c"

#include "flags.h"
#include "geo_lookup.h"

#define SOCKET_PATH "/tmp/myserver.sock"
#define BUFFER_SIZE 1024

// Количество элементов в массиве флагов
#define FLAGS_COUNT (sizeof(flags) / sizeof(flags[0]))

/**
 * Получает информацию о DNS для указанного домена.
 *
 * Эта функция выполняет команду `dig` для получения списка IP-адресов, связанных с доменом.
 * IP-адреса сохраняются в предоставленный буфер `ips`.
 *
 * @param domain Указатель на строку с доменом, для которого нужно получить IP-адреса.
 * @param ips Указатель на буфер, куда будут записаны IP-адреса.
 * @param ips_size Размер буфера `ips`.
 */
void get_dns_info(const char *domain, char *ips, size_t ips_size);

/**
 * Обрабатывает соединение с клиентом.
 *
 * Эта функция получает запрос от клиента, извлекает домен из запроса,
 * получает IP-адреса для домена, находит информацию о стране и возвращает её в формате JSON.
 *
 * @param client_sock Дескриптор сокета клиента.
 * @param mmdb Указатель на структуру базы данных MaxMind, предварительно открытая.
 */
void handle_client(int client_sock, const MMDB_s *mmdb);

/**
 * Получает информацию о стране и флаге по IP-адресу.
 *
 * Эта функция использует IP-адрес для получения кода страны, затем находит
 * соответствующую информацию о флаге по коду страны.
 *
 * @param mmdb Указатель на структуру базы данных MaxMind, предварительно открытая.
 * @param ip Указатель на строку с IP-адресом.
 * @param country_code Указатель на буфер для записи кода страны.
 * @param country_code_size Размер буфера `country_code`.
 *
 * @return Указатель на структуру `Flag`, содержащую информацию о флаге, или NULL, если флаг не найден.
 */
const Flag *get_geo_info(const MMDB_s *mmdb, const char *ip, char *country_code, size_t country_code_size);

int main()
{
    char *db_path = "./GeoLite2-City.mmdb";
    int server_sock;
    struct sockaddr_un server_addr;
    MMDB_s mmdb;
    int mmdb_error;

    // Открываем базу данных GeoLite2
    mmdb_error = MMDB_open(db_path, 0, &mmdb);
    if (mmdb_error != MMDB_SUCCESS)
    {
        fprintf(stderr, "Не удалось открыть файл базы данныъ MMDB - %s\n", MMDB_strerror(mmdb_error));
        return EXIT_FAILURE;
    }

    // Создаем Unix-сокет
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("socket");   // Печатаем сообщение об ошибке, если создание сокета не удалась
        MMDB_close(&mmdb);  // Закрываем MMDB перед выходом
        exit(EXIT_FAILURE); // Завершаем программу с кодом ошибки
    }

    // Настраиваем адрес сервера
    memset(&server_addr, 0, sizeof(server_addr));                                 // Очищаем структуру адреса
    server_addr.sun_family = AF_UNIX;                                             // Устанавливаем семейство адресов в Unix
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1); // Копируем путь к сокету

    // Удаляем старый сокет, если он существует
    unlink(SOCKET_PATH);

    // Привязываем сокет к адресу
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");     // Печатаем сообщение об ошибке, если привязка сокета не удалась
        close(server_sock); // Закрываем сокет
        MMDB_close(&mmdb);  // Закрываем MMDB перед выходом
        exit(EXIT_FAILURE); // Завершаем программу с кодом ошибки
    }

    // Пример изменения прав доступа к сокету
    if (chmod(SOCKET_PATH, 0777) < 0)
    {
        perror("chmod");
    }

    // Слушаем входящие соединения
    if (listen(server_sock, 5) < 0)
    {
        perror("listen");   // Печатаем сообщение об ошибке, если не удалось начать прослушивание
        close(server_sock); // Закрываем сокет
        MMDB_close(&mmdb);  // Закрываем MMDB перед выходом
        exit(EXIT_FAILURE); // Завершаем программу с кодом ошибки
    }

    // Информируем пользователя, что сервер начал слушать
    printf("Unix-сервер слушает на сокете %s\n", SOCKET_PATH);

    // Основной цикл обработки входящих соединений
    while (1)
    {
        int client_sock = accept(server_sock, NULL, NULL); // Принимаем входящее соединение
        if (client_sock < 0)
        {
            perror("accept"); // Печатаем сообщение об ошибке, если не удалось принять соединение
            continue;         // Продолжаем цикл, не закрывая серверный сокет
        }

        handle_client(client_sock, &mmdb); // Обрабатываем запрос клиента
        close(client_sock);                // Закрываем соединение с клиентом
    }

    // Закрываем серверный сокет
    close(server_sock);

    // Закрываем базу данных MMDB
    MMDB_close(&mmdb);

    return 0; // Завершаем программу
}

void get_dns_info(const char *domain, char *ips, size_t ips_size)
{
    FILE *fp;
    char command[2048];
    char buffer[BUFFER_SIZE];

    printf("Получена строка: %s\n", domain);

    snprintf(command, sizeof(command), "dig +short %s", domain);
    fp = popen(command, "r");
    if (fp == NULL)
    {
        perror("popen");
        return;
    }

    ips[0] = '\0';
    while (fgets(buffer, sizeof(buffer) - 1, fp) != NULL)
    {
        buffer[strcspn(buffer, "\n")] = '\0';
        strncat(ips, buffer, ips_size - strlen(ips) - 1);
        strncat(ips, " ", ips_size - strlen(ips) - 1);
    }

    pclose(fp);
}

void handle_client(int client_sock, const MMDB_s *mmdb)
{
    // Буферы для хранения данных
    char buffer[BUFFER_SIZE];        // Буфер для приема данных от клиента
    char ips[BUFFER_SIZE];           // Буфер для хранения IP-адресов
    char first_ip[BUFFER_SIZE];      // Буфер для хранения первого IP-адреса
    char country_code[BUFFER_SIZE];  // Буфер для хранения кода страны
    char json_response[BUFFER_SIZE]; // Буфер для хранения JSON-ответа

    // Получаем данные от клиента
    int recv_len = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (recv_len < 0)
    {
        perror("recv"); // Печатаем сообщение об ошибке, если прием данных не удался
        return;         // Выходим из функции
    }
    buffer[recv_len] = '\0'; // Завершаем строку нулевым символом

    printf("Запрос из браузера: %s\n", buffer);

    // Извлекаем домен из строки запроса
    char *domain_start = strstr(buffer, "/what-is-country/");
    if (domain_start != NULL)
    {
        domain_start += strlen("/what-is-country/"); // Сдвигаем указатель на начало домена

        // Копируем домен в буфер
        char domain[BUFFER_SIZE];
        strncpy(domain, domain_start, sizeof(domain) - 1);
        domain[sizeof(domain) - 1] = '\0'; // Завершаем строку нулевым символом

        // Убираем возможные символы новой строки
        char *newline_pos = strchr(domain, '\n');
        if (newline_pos != NULL)
            *newline_pos = '\0';

        // Получаем информацию о DNS (в данном случае IP-адреса)
        get_dns_info(domain, ips, sizeof(ips));

        // Извлекаем первый IP-адрес из строки IP-адресов
        char *first_ip_end = strchr(ips, ' '); // Ищем первый пробел в строке IP-адресов
        if (first_ip_end != NULL)
        {
            size_t len = first_ip_end - ips; // Определяем длину первого IP-адреса
            if (len >= sizeof(first_ip))
            {
                len = sizeof(first_ip) - 1; // Ограничиваем длину, если она больше размера буфера
            }
            strncpy(first_ip, ips, len); // Копируем первый IP-адрес в буфер
            first_ip[len] = '\0';        // Завершаем строку нулевым символом
        }
        else
        {
            snprintf(first_ip, sizeof(first_ip), "%s", ips); // Если пробел не найден, копируем всю строку
        }

        // Получаем информацию о стране и флаге для первого IP-адреса
        const Flag *flag_struct = get_geo_info(mmdb, first_ip, country_code, sizeof(country_code));

        // Создаем новый JSON-объект
        struct json_object *json_obj = json_object_new_object();
        struct json_object *json_ips = json_object_new_string(ips); // Создаем JSON-строку для IP-адресов
        json_object_object_add(json_obj, "ips", json_ips);          // Добавляем IP-адреса в JSON

        // Если получена информация о флаге, добавляем её в JSON
        if (flag_struct)
        {
            json_object_object_add(json_obj, "flagImg", json_object_new_string(flag_struct->flag_img)); // Добавляем изображение флага
            json_object_object_add(json_obj, "countryName", json_object_new_string(flag_struct->name)); // Добавляем название страны
        }

        // Преобразуем JSON-объект в строку и сохраняем её в буфере ответа
        snprintf(json_response, sizeof(json_response), "%s", json_object_to_json_string(json_obj));

        size_t response_size = snprintf(NULL, 0,
                                        "HTTP/1.1 200 OK\r\n"
                                        "Content-Type: application/json\r\n"
                                        "Content-Length: %ld\r\n"
                                        "Access-Control-Allow-Origin: *\r\n" // Добавляем заголовок CORS
                                        "Connection: close\r\n"
                                        "\r\n%s",
                                        strlen(json_response), json_response);

        char *response = malloc(response_size + 1); // +1 для нулевого байта

        if (response == NULL)
        {
            perror("malloc");
            return;
        }

        snprintf(response, response_size + 1,
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: application/json\r\n"
                 "Content-Length: %ld\r\n"
                 "Access-Control-Allow-Origin: *\r\n" // Добавляем заголовок CORS
                 "Connection: close\r\n"
                 "\r\n%s",
                 strlen(json_response), json_response);

        // Отправляем ответ
        send(client_sock, response, response_size, 0);

        // Освобождаем память
        free(response);

        // Освобождаем ресурсы, связанные с JSON-объектом
        json_object_put(json_obj);
    }
    else
    {
        send(client_sock, "Invalid request format.\n", strlen("Invalid request format.\n"), 0);
        fprintf(stderr, "Invalid request format.\n");
    }
}

const Flag *get_geo_info(const MMDB_s *mmdb, const char *ip, char *country_code, size_t country_code_size)
{
    // Получаем страну по IP-адресу
    country_code = get_country_from_ip(ip, *mmdb);

    // Найти флаг по коду страны
    for (size_t i = 0; i < FLAGS_COUNT; ++i)
    {
        if (strcmp(flags[i].key, country_code) == 0)
        {
            return &flags[i];
        }
    }

    return NULL;
}

// gcc -o unix-server unix-server.c geo_lookup.c -lmaxminddb -ljson-c