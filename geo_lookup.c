#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <maxminddb.h>

char *print_entry_data(MMDB_entry_data_s *entry_data, int *iso_code);
void print_entry_data_list(MMDB_entry_data_list_s *entry_data_list);
char *get_country_from_ip(const char *ip_address, MMDB_s mmdb);


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
char *get_country_from_ip(const char *ip_address, MMDB_s mmdb)
{
    MMDB_lookup_result_s lookup_result;
    MMDB_entry_data_list_s *entry_data;
    int gai_error; // Переменная для хранения ошибок gai
    int mmdb_error;
    int status;
    char *country_info = NULL;
    int iso_code = 0;

    // Выполняем поиск по IP-адресу
    lookup_result = MMDB_lookup_string(&mmdb, ip_address, &gai_error, &mmdb_error);

    if (lookup_result.found_entry)
    {
        // Получаем данные из найденной записи
        status = MMDB_get_entry_data_list(&lookup_result.entry, &entry_data);

        if (status == MMDB_SUCCESS)
        {
            // Сначала печатаем данные, чтобы получить информацию о стране
            // А затем конвертируем в строку
            MMDB_entry_data_list_s *current_entry_data = entry_data;
            while (current_entry_data != NULL)
            {
                // Попробуем получить строку информации из каждого элемента списка
                char *result = print_entry_data(&current_entry_data->entry_data, &iso_code);
                if (result != NULL)
                {
                    country_info = result;
                    break;
                }
                current_entry_data = current_entry_data->next;
            }
        }
        else
        {
            printf("Ошибка при получении данных: %s\n", MMDB_strerror(status));
        }
    }
    else
    {
        printf("IP-адрес не найден в базе данных.\n");
    }

    return country_info;
}

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
char *print_entry_data(MMDB_entry_data_s *entry_data, int *iso_code)
{
    char *result = NULL;

    if (entry_data->has_data)
    {
        if (entry_data->type == MMDB_DATA_TYPE_UTF8_STRING)
        {
            // Если iso_code равно 1, вернуть строку и изменить iso_code на 3
            if (*iso_code == 1)
            {
                result = malloc(entry_data->data_size + 1); // +1 для нуль-терминирующего символа
                if (result != NULL)
                {
                    memcpy(result, entry_data->utf8_string, entry_data->data_size);
                    result[entry_data->data_size] = '\0'; // Нуль-терминируем строку
                    *iso_code = 3;
                }
            }
            // Сравниваем строку с "iso_code"
            if (strncmp(entry_data->utf8_string, "iso_code", entry_data->data_size) == 0)
            {
                *iso_code = 1;
            }
        }
    }
    else
    {
        // Данные отсутствуют
        return NULL;
    }

    return result;
}

/**
 * Печатает список данных типа MMDB_entry_data_list_s.
 * 
 * Эта функция перебирает список данных типа MMDB_entry_data_list_s и вызывает `print_entry_data`
 * для каждого элемента списка. 
 * Цикл завершается, если `iso_code` становится равным 3.
 * 
 * @param {MMDB_entry_data_list_s *} entry_data_list - Указатель на начало списка данных.
 */
void print_entry_data_list(MMDB_entry_data_list_s *entry_data_list)
{
    MMDB_entry_data_list_s *current_entry_data = entry_data_list;
    int iso_code = 0;
    while (current_entry_data != NULL)
    {
        // Если iso_code равно 3, завершить цикл
        if (iso_code == 3) return;
        print_entry_data(&current_entry_data->entry_data, &iso_code);
        current_entry_data = current_entry_data->next;
    }
}
