
# Unix Socket DNS Server

## Описание
Этот проект представляет собой сервер на основе Unix-сокетов, который получает домен от клиента, находит его IP-адреса с помощью команды `dig`, а затем, используя базу данных MaxMind GeoLite2, определяет страну для одного из IP-адресов и возвращает информацию о стране и строку в формате base64 для изображения флага в формате JSON.

## Используемые технологии
- **C** — Основной язык программирования.
- **Unix-сокеты** — Используются для обмена сообщениями между сервером и клиентом.
- **MaxMindDB (GeoLite2)** — База данных для геолокации IP-адресов.
- **JSON-C** — Библиотека для работы с JSON в C.
- **libmaxminddb** — Для работы с базой данных MaxMind.

## Как использовать

### Сборка
1. Убедитесь, что у вас установлены необходимые библиотеки:
   - `libmaxminddb`
   - `libjson-c`

2. Скомпилируйте проект с помощью следующей команды:
   ```bash
   gcc unix-server.c geo_lookup.c -o unix-server -lmaxminddb -ljson-c
   ```

### Запуск сервера
1. Убедитесь, что у вас есть база данных MaxMind GeoLite2 в формате MMDB. Вы можете скачать её с [официального сайта MaxMind](https://dev.maxmind.com/geoip/geoip2/geolite2/).
2. Поместите файл базы данных (`GeoLite2-City.mmdb`) в ту же директорию, где находится исполняемый файл.

3. Запустите сервер:
   ```bash
   ./unix-server
   ```

После успешного запуска сервер будет слушать на Unix-сокете `/tmp/myserver.sock`.

### Пример запроса
С помощью клиента (например, через `curl`), вы можете отправить запрос на сервер:

```bash
echo -n "/what-is-country/example.com" | nc -U /tmp/myserver.sock
```

Сервер ответит JSON-объектом с информацией о домене, IP-адресе, стране и изображении флага.

Пример ответа:
```json
{
    "ips": "93.184.216.34 ",
    "flagImg": "iVBORw0KGgoAAAANSUhEUgAAABAAAAAMCAMAAABcOc2zAAABHVBMVEWMGSlPT3daWn9dXICMHCyNIi54ER2RHy54ERxXVnpQT3Z5M0+aJzqIGSmQIjBuDRaQIS2QIS5+FSB8Ex5zDxpnZpBjYozFWGXx2Nvcm6PDUF+7Q1HgrLLKbnnz3uHu09bUjpbk0dNmZY5vbpXmub+/TFrgqbCzPUqoWGPKmaBXVntqaZJfXolWVYPdoqrViJLp2d7ir7bFeIKvg4m+VGHBXGfXur6fSFPmxMjjtbqYSVPHrrGqeoCxanO0RFBfXoJQT3WZYHral5+fb4lNS3GpjKPm3uXg3eWWTmq5ucyqnLDAX2qJN0CznJ90dI7KY3HUk5rsyc7epKvTgIuYZmuvXmmNT1WgTVfUhI+wdXy5i5K+pqnAkpfAZG7RiZFR4mcfAAAAFXRSTlMgOpHITDGTct3b4bmVPss6PstvucjjrrfjAAAAxklEQVQIHQXBBUICQRQA0A8CuyghyMzsbHeQS0h3qZRBl/c/Bu9BIPgUfY6+xJhQGOCVScEA64aBF3WvsnfdnneFrJFDGH3y31tK6Ym0IYsQ0nMzRf4t27Ypd2Go5zHCE8kSeEIEqwtThPOFwmhsKpwsc2IbNl8fvj/XxJJUc4SluoYq4QnhBUtSi63yz3IFF84sUrHJKS36r9pKFRqk9CeqlXenX5PufWcHHa93bLjnW13zm5rWOUCCiQOEQ+nIG8uymUjyAUFxHXDIwv3pAAAAAElFTkSuQmCC",
    "countryName": "United States (US)"
}
```

## Файлы проекта

1. **unix-server.c** — Основной файл, который содержит логику создания Unix-сокета, обработки клиентских запросов и взаимодействия с базой данных MaxMind для получения информации о стране.
2. **geo_lookup.c** — Файл, отвечающий за работу с базой данных MaxMind. Определяет страну по IP-адресу.
3. **geo_lookup.h** — Заголовочный файл для работы с функциями геолокации.

## Как работает сервер

1. **Прием запроса:** Сервер принимает запрос клиента по Unix-сокету. Ожидаемый формат запроса — строка, содержащая доменное имя.
2. **Получение IP-адресов:** С помощью команды `dig` сервер получает IP-адреса, связанные с доменным именем.
3. **Геолокация:** Используя библиотеку MaxMind, сервер определяет страну по IP-адресу.
4. **Ответ в формате JSON:** Сервер возвращает информацию о домене, IP-адресе, стране и флаге в формате JSON.

## Зависимости

Для работы сервера необходимы следующие библиотеки:
- **libmaxminddb** — Для работы с базой данных GeoLite2 от MaxMind.
- **libjson-c** — Для работы с форматом JSON.

Установка на Ubuntu:
```bash
sudo apt-get install libmaxminddb-dev libjson-c-dev
```

## Лицензия
Проект использует базу данных GeoLite2, предоставленную MaxMind под лицензией [Creative Commons Attribution-ShareAlike 4.0 International License](https://creativecommons.org/licenses/by-sa/4.0/).

Этот файл README предоставляет полное руководство на русском языке, объясняя работу сервера, его сборку, запуск и обработку запросов.
