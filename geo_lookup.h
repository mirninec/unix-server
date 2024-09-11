#ifndef GEO_LOOKUP_H
#define GEO_LOOKUP_H

#include <maxminddb.h>

/**
 * Получает информацию о стране по IP-адресу и возвращает её как строку.
 *
 * Эта функция открывает базу данных MaxMind, ищет информацию по указанному IP-адресу,
 * затем получает данные о стране и возвращает их в виде строки.
 *
 * @param {const char *} ip_address - IP-адрес, для которого необходимо получить информацию.
 * @param {MMDB_s} mmdb - Структура базы данных MaxMind, предварительно открытая.
 *
 * @return {char *} - Возвращает строку с информацией о стране или NULL в случае ошибки.
 */
char *get_country_from_ip(const char *ip_address, MMDB_s mmdb);

/**
 * Обрабатывает данные типа MMDB_entry_data_s и возвращает строковый результат.
 *
 * Эта функция выделяет память для строки, содержащей данные типа MMDB_ENTRY_DATA_S.
 * Если текущий тип данных - UTF8_STRING и значение `iso_code` равно 1,
 * возвращает строку и изменяет `iso_code` на 3.
 * Если текущий тип данных - UTF8_STRING и строка равна "iso_code",
 * устанавливает `iso_code` в 1.
 *
 * @param {MMDB_entry_data_s *} entry_data - Указатель на структуру данных MMDB.
 * @param {int *} iso_code - Указатель на целое число, используемое для определения, должна ли быть возвращена строка.
 *
 * @return {char *} - Возвращает строку, если данные доступны и тип данных - UTF8_STRING.
 *                    Возвращает NULL, если данные отсутствуют или не соответствуют условиям.
 */
char *print_entry_data(MMDB_entry_data_s *entry_data, int *iso_code);

/**
 * Печатает список данных типа MMDB_entry_data_list_s.
 *
 * Эта функция перебирает список данных типа MMDB_entry_data_list_s и вызывает `print_entry_data`
 * для каждого элемента списка.
 * Цикл завершается, если `iso_code` становится равным 3.
 *
 * @param {MMDB_entry_data_list_s *} entry_data_list - Указатель на начало списка данных.
 */
void print_entry_data_list(MMDB_entry_data_list_s *entry_data_list);

#endif // GEO_LOOKUP_H
