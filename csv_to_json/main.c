#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DATA_SIZE 1024
#define MAX_KEYS_SIZE 128
#define DELIMITER ";" // Разделитель, используемый с csv файлах


int main(int argc, char *argv[]) {
    // Открываем файл, переданный пользователем
    FILE *inputFile = fopen(argv[1], "r");

    // Проверяем открылся ли
    if (inputFile == NULL) {
        printf("Error opening %s file.\nUse Parser <filename>", argv[1]);
        return 1;
    }

    // Открываем выходной файл на запись
    FILE *jsonFile = fopen("output.json", "w");

    // Проверяем открылся ли
    if (jsonFile == NULL) {
        fclose(inputFile);
        printf("Error opening the output.json file.\n");
        return 1;
    }

    char *keys[MAX_KEYS_SIZE];
    int keycnt = 0;

    char line[MAX_DATA_SIZE];
    int linecnt = 0;

    char *token;

    // Получаем строку с ключами и проверяем, что получили
    char keys_line[MAX_DATA_SIZE];
    if (fgets(keys_line, MAX_DATA_SIZE, inputFile) == NULL) {
        printf("Error read keys from input file");
        fclose(inputFile);
        fclose(jsonFile);
        return 1;
    }
    // Разбиваем ключи по разделителю
    token = strtok(keys_line, DELIMITER);
    while (token != NULL) {
        keys[keycnt] = strdup(token);
        keycnt++;
        token = strtok(NULL, DELIMITER);
    }
    // Записываем json файл
    fputs("[\n\t", jsonFile);
    while (fgets(line, MAX_DATA_SIZE, inputFile) != NULL) {
        if (linecnt > 0)
            fprintf(jsonFile, ",\n\t");
        fprintf(jsonFile, "{\n\t\t");

        token = strtok(line, DELIMITER);
        for (int i = 0; token != NULL && i < (keycnt - 1); i++) {
            fprintf(jsonFile, "\"%s\": \"%s\"", keys[i], token);

            token = strtok(NULL, DELIMITER);

            if (i < (keycnt - 2))
                fprintf(jsonFile, ",\n\t\t");
        }

        fprintf(jsonFile, "\n\t}");
        linecnt++;
    }
    fputs("\n]", jsonFile);

    //Закрываем файл && очищаем память
    fclose(jsonFile);
    fclose(inputFile);
    for (int i = 0; i < keycnt; i++)
        free(keys[i]);
    printf("Data written to output.json successfully.\n");

    return 0;
}
