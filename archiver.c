#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

union record {
  char charptr[512];
  struct header {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[8];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
  } header;
};

int calculate_time(char *date, char *time) {
  char *ptr, *year, *month, *day, *hour, *minutes, *seconds;
  struct tm info;
  int cv;

  ptr = strtok(date, "-");
  year = ptr;
  ptr = strtok(NULL, "-");
  month = ptr;
  ptr = strtok(NULL, "-");
  day = ptr;
  ptr = strtok(time, ":");
  hour = ptr;
  ptr = strtok(NULL, ":");
  minutes = ptr;
  ptr = strtok(NULL, ".");
  seconds = ptr;

  info.tm_year = atoi(year) - 1900;
  info.tm_mon = atoi(month) - 1;
  info.tm_mday = atoi(day);
  info.tm_hour = atoi(hour);
  info.tm_min = atoi(minutes);
  info.tm_sec = atoi(seconds);
  info.tm_isdst = -1;
  cv = mktime(&info);
  return cv;
}

int trans_permissions(char *permissions) {
  int sum1 = 0, sum2 = 0, sum3 = 0;
  if (permissions[1] == 'r') sum1 += 4;
  if (permissions[2] == 'w') sum1 += 2;
  if (permissions[3] == 'x') sum1 += 1;
  if (permissions[4] == 'r') sum2 += 4;
  if (permissions[5] == 'w') sum2 += 2;
  if (permissions[6] == 'x') sum2 += 1;
  if (permissions[7] == 'r') sum3 += 4;
  if (permissions[8] == 'w') sum3 += 2;
  if (permissions[9] == 'x') sum3 += 1;
  return ((sum1 * 100) + (sum2 * 10) + sum3);
}

int octal2decimal(int x) {
  int i = 0, j = 0, r;
  while (x != 0) {
    r = x % 10;
    x = x / 10;
    i = i + r * pow(8, j);
    j++;
  }
  return i;
}

void write_empty_blocks(FILE *fp) {
  union record meta;
  memset(&meta, 0, 512);
  fwrite(&meta, sizeof(meta), 1, fp);
  fwrite(&meta, sizeof(meta), 1, fp);
  fwrite("\n", sizeof(char), 1, fp);
}

unsigned int calculate_checksum(union record meta) {
  unsigned int sum = 0;
  unsigned char *p = (unsigned char *)&meta.header;
  for (unsigned int i = 0; i < sizeof(meta); i++) {
    sum += p[i];
  }
  sum += 256;
  return sum;
}

void write_meta(union record meta, FILE *fp, char *name, int long_time,
                int perm_new, char *owner_name, char *owner_group, int uid,
                int gid, char *size) {
  memset(&meta, 0, 512);
  sprintf(meta.header.name, "%s", name);
  sprintf(meta.header.mode, "0000%d", perm_new);
  sprintf(meta.header.uid, "000%o", uid);
  sprintf(meta.header.gid, "000%o", gid);
  sprintf(meta.header.size, "%011o", atoi(size));
  sprintf(meta.header.mtime, "%011o", long_time);
  sprintf(meta.header.linkname, "%s", name);
  sprintf(meta.header.magic, "GNUtar ");
  sprintf(meta.header.uname, "%s", owner_name);
  sprintf(meta.header.gname, "%s", owner_group);
  unsigned int sum = calculate_checksum(meta);
  sprintf(meta.header.chksum, "%07o", sum);
  fwrite(&meta, sizeof(meta), 1, fp);
}

void write_data(FILE *fp, char *name_dir, char *name, char *size) {
  FILE *file_prl;           // fisierul la care am ajuns de pus in arhiva
  int size_i = atoi(size);  // transformam size in int
  char dir_copy[200];
  int y;
  strcpy(dir_copy, name_dir);  // copie dupa director
  strcat(dir_copy,
         name);  // se concateneaza directorul cu fisierul pentru a-l deschide
  file_prl = fopen(dir_copy, "rb");

  y = fgetc(file_prl);
  while (y != EOF) {  // se citeste caracter cu caracter
    fputc(y, fp);
    y = fgetc(file_prl);
  }

  fclose(file_prl);

  int i = 1;
  if (size_i % 512 !=
      0) {  // se cauta multipul lui 512 mai mare ca size_i si se adauga '/0'
    while (size_i > (512 * i)) {
      i++;
    }

    while (size_i != (512 * i)) {
      fputc(0, fp);
      size_i++;
    }
  }
}

// functie pentru create
void create(char *name_dir, FILE *fp) {
  char permissions[100], owner_name[100], owner_group[100], date[100],
      time_m[100], temp[100], name[100];
  FILE *files, *usermap;
  char no_links[100];
  char size[100];
  int count_lines = 0, count_lines2 = 0, perm_new, long_time, uid, gid;
  char c = 0;
  union record meta;

  files = fopen("files.txt", "r");
  usermap = fopen("usermap.txt", "r");

  for (c = fgetc(files); c != EOF; c = fgetc(files)) {
    if (c == '\n') {
      count_lines++;
    }
  }

  c = 0;
  for (c = fgetc(usermap); c != EOF; c = fgetc(usermap)) {
    if (c == '\n') {
      count_lines2++;
    }
  }

  rewind(usermap);
  rewind(files);

  for (int i = 0; i < count_lines; i++) {
    fscanf(files, "%s", permissions);
    fscanf(files, "%s", no_links);
    fscanf(files, "%s", owner_name);
    fscanf(files, "%s", owner_group);
    fscanf(files, "%s", size);
    fscanf(files, "%s %s", date, time_m);
    fscanf(files, "%s", temp);
    fscanf(files, "%s", name);
    perm_new = trans_permissions(permissions);
    long_time = calculate_time(date, time_m);
    uid = 1000;
    gid = 1000;
    write_meta(meta, fp, name, long_time, perm_new, owner_name, owner_group,
               uid, gid, size);
    write_data(fp, name_dir, name, size);
  }

  write_empty_blocks(fp);
  fclose(usermap);
  fclose(files);
}

// functie pentru list
void list(char *name_archive) {
  FILE *file;
  char meta[512];
  int block, i;
  int size_i, size_decimal;

  if (fopen(name_archive, "r") == NULL) {
    printf("> File not found!\n");
  } else {
    file = fopen(name_archive, "r");
    while (fread(meta, 512, 1, file)) {
      if (!strcmp(&meta[257], "GNUtar") || !strcmp(&meta[257], "ustar")) {
        printf("> %s\n", meta);
      }
    }
    fclose(file);
  }
}

// functie pentru extract
void extract(char *name_archive, char *file_extracted) {
  FILE *file, *new_file;
  // union record meta;
  char meta[512];
  int size, y;
  char str[] = "extracted_";
  char *result = malloc(strlen(str) + strlen(file_extracted) + 1);
  strcpy(result, str);
  strcat(result, file_extracted);
  file = fopen(name_archive, "rb");
  new_file = fopen(result, "wb");

  while (fread(meta, 512, 1, file)) {
    if ((!(strcmp(&meta[257], "GNUtar")) || !(strcmp(&meta[257], "ustar"))) &&
        strcmp(&meta[0], file_extracted) == 0) {
      size = atoi(&meta[124]);
      size = octal2decimal(size);
      y = fgetc(file);

      while (y != EOF && size > 0) {
        fputc(y, new_file);
        y = fgetc(file);
        size--;
      }
      printf("> File extracted!\n");
      break;
    }
  }

  free(result);
  fclose(file);
  fclose(new_file);
}

int main() {
  char *command, *name_archive, *name_dir, *file_extracted, line[512];
  int len;
  FILE *fp;

  while (1) {
    fgets(line, 512, stdin);
    len = strlen(line);
    if (line[len - 1] == '\n') line[len - 1] = '\0';
    command = strtok(line, " ");

    if (strcmp(command, "create") == 0) {
      // se compara functia data cu compare
      name_archive = strtok(NULL, " ");
      name_dir = strtok(NULL, " ");
      fp = fopen(name_archive, "wb");
      create(name_dir, fp);
      fclose(fp);
      printf("> Done!\n");

    } else if (strcmp(command, "list") == 0) {
      // se compara functia data cu list
      name_archive = strtok(NULL, "\n");
      list(name_archive);

    } else if (strcmp(command, "extract") ==
             0) {
      // se compara functia data cu extract
      file_extracted = strtok(NULL, " ");
      name_archive = strtok(NULL, " ");
      extract(name_archive, file_extracted);

    } else if (strcmp(command, "exit") == 0) {
      // se compara functia data cu exit
      return 0;

    } else {
      printf("> Wrong command!");
      printf("\n");
    }
  }
  return 0;
}
