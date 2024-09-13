#ifndef UNIX_SERVER_H
#define UNIX_SERVER_H

#include <maxminddb.h> // For MaxMindDB database
#include "flags.h"     // For Flag structure

#define SOCKET_PATH "/tmp/myserver.sock"
#define BUFFER_SIZE 1024

/**
 * @brief Получает информацию о DNS для заданного домена и возвращает только IPv4-адреса.
 *
 * Функция выполняет команду `dig +short` для получения информации о DNS и объединяет
 * только корректные IPv4-адреса в одну строку. Данные, не являющиеся IPv4-адресами, исключаются.
 *
 * @param domain Строка с именем домена.
 * @param ips Буфер для записи результирующей строки с IP-адресами.
 * @param ips_size Размер буфера для IP-адресов.
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
 * @brief Получает информацию о стране и флаге по IP-адресу.
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

/**
 * @brief Проверяет, является ли строка валидным IPv4-адресом.
 *
 * Функция использует `inet_pton` для проверки того, соответствует ли данная строка формату IPv4.
 *
 * @param ip Строка с предполагаемым IPv4-адресом.
 * @return int Возвращает 1, если строка является корректным IPv4-адресом, и 0 в противном случае.
 */
int is_valid_ipv4(const char *ip);

#endif /* UNIX_SERVER_H */
