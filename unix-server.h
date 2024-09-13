#ifndef UNIX_SERVER_H
#define UNIX_SERVER_H

#include <maxminddb.h> // For MaxMindDB database
#include "flags.h"     // For Flag structure

#define SOCKET_PATH "/tmp/myserver.sock"
#define BUFFER_SIZE 1024

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

#endif /* UNIX_SERVER_H */
