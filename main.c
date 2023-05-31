#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

typedef struct {
    cJSON *json;
} DataItem;

// Рекурсивная функция записи заголовков
void rec_write_header(cJSON *cjson, FILE *file) {
    int flag = 1;
    if (cjson->child != NULL) {
        flag = 0;
        rec_write_header(cjson->child, file);
    }
    if (cjson->next != NULL) {
        rec_write_header(cjson->next, file);
    }
    if (flag)
        fprintf(file, "%s,", cjson->string);
}

// Рекурсивная функция записи основных данных
void rec_write_data(cJSON *cjson, FILE *file) {
    int flag = 1;
    if (cjson->child != NULL) {
        flag = 0;
        rec_write_data(cjson->child, file);
    }
    if (cjson->next != NULL) {
        rec_write_data(cjson->next, file);
    }
    if (flag) {
        if (cjson->type == cJSON_String) {
            fprintf(file, "%s", cjson->valuestring);
        } else if (cjson->type == cJSON_Number) {
            double value = cjson->valuedouble;
            fprintf(file, "%g", value);
        }
        fprintf(file, ",");
    }
}

int main(int argc, char *argv[]) {
    // Открываем файл, переданный пользователем
    FILE *inputFile = fopen(argv[1], "r");

    // Проверяем открылся ли
    if (inputFile == NULL) {
        printf("Error opening %s file.\nUse Parser <filename>", argv[1]);
        return 1;
    }

    // Определяем размер файла
    fseek(inputFile, 0, SEEK_END);
    long fileSize = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);

    // Считываем файл в строку
    char *jsonString = (char *) malloc((fileSize + 1) * sizeof(char));
    fread(jsonString, sizeof(char), fileSize, inputFile);
    jsonString[fileSize] = '\0';

    fclose(inputFile);

    // Парсим строку
    cJSON *root = cJSON_Parse(jsonString);

    // Проверяем пропарсилась ли
    if (root == NULL) {
        printf("Error parsing JSON string.\n");
        free(jsonString);
        return 1;
    }

    // Проверяем соответствует ли json стандарту
    if (!cJSON_IsArray(root)) {
        printf("Error: Root JSON structure should be an array.\n");
        cJSON_Delete(root);
        free(jsonString);
        return 1;
    }

    // Разбиваем на массив элементов
    int numItems = cJSON_GetArraySize(root);
    DataItem *items = (DataItem *) malloc(numItems * sizeof(DataItem));

    for (int i = 0; i < numItems; i++) {
        cJSON *item = cJSON_GetArrayItem(root, i);
        items[i].json = item;
    }

    // Открываем выходной файл на запись
    FILE *csvFile = fopen("output.csv", "w");

    // Проверяем открылся ли
    if (csvFile == NULL) {
        printf("Error opening the output.csv file.\n");
        cJSON_Delete(root);
        free(items);
        free(jsonString);
        return 1;
    }

    // Записываем заголовки
    rec_write_header(items[0].json->child, csvFile);
    fprintf(csvFile, "\n");

    // Записываем основные данные
    for (int i = 0; i < numItems; i++) {
        rec_write_data(items[i].json->child, csvFile);
        fprintf(csvFile, "\n");
    }

    //Закрываем файл
    fclose(csvFile);

    printf("Data written to output.csv successfully.\n");

    // Очищаем память
    cJSON_Delete(root);
    free(items);
    free(jsonString);

    return 0;
}
