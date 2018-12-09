#include <stdio.h>
#include <string.h>

char output_name[25];
int num_files;

void pack(char filenames[][256]) {
	FILE *fout = fopen(output_name, "w");
	for (int i = 0; i < num_files; ++i) {
		FILE *fin = fopen(filenames[i], "r");
		fseek(fin, 0L, SEEK_END);
		size_t size = ftell(fin);
		fseek(fin, 0L, SEEK_SET);

		char buf[size];
		memset(buf, 0, size);
		fread(buf, 1, size, fin);
		fprintf(fout, "%ld%s%ld", strlen(filenames[i]), filenames[i], size);
		fwrite(buf, 1, size, fout);
		fclose(fin);
	}
	fclose(fout);
}

void unpack() {
	FILE *fin = fopen(output_name, "r");
	while (!feof(fin)) {
		int name_size;
		int file_size;
		fscanf(fin, "%d", &name_size);
		char file_name[256];
		memset(file_name, 0, sizeof(file_name));
		size_t read = fread(file_name, sizeof(char), name_size, fin);
		if (read == name_size) {
			fscanf(fin, "%d", &file_size);
		
			FILE *fout = fopen(file_name, "w");

			char buf[file_size];
			memset(buf, 0, file_size);
			fread(buf, 1, file_size, fin);
			fwrite(buf, 1, file_size, fout);
			fclose(fout);
		}
	}
	fclose(fin);
}

void list() {
	FILE *fin = fopen(output_name, "r");
	while (!feof(fin)) {
		int name_size;
		int file_size;
		fscanf(fin, "%d", &name_size);
		char file_name[256];
		memset(file_name, 0, sizeof(file_name));
		size_t read = fread(file_name, sizeof(char), name_size, fin);
		if (read == name_size) {
			fscanf(fin, "%d", &file_size);
		
			printf("%s\n", file_name);
			char buf[file_size];
			memset(buf, 0, file_size);
			fread(buf, 1, file_size, fin);
		}
	}
	fclose(fin);
}

int main(int argc, char **argv) {
	if (argc == 1) {
		//printf("Welcome to the file archiver!\n
		//	Usage: ./arc ");
	}

	else if (argc > 2) {
		char* arg = argv[1];
		if (arg[0] == '-') {
			switch (arg[1]) {
				case 'c':
					strcpy(output_name, argv[2]);
					num_files = argc - 3;
					if (num_files > 0) {
						char files[num_files][256];
						for (int j = 3; j-3 < num_files; j ++) {
							strcpy(files[j - 3], argv[j]);
						}
						pack(files);
					}
					break;
				case 'x':
					strcpy(output_name, argv[2]);
					unpack();
					break;
				case 'l':
					strcpy(output_name, argv[2]);
					list();
					break;
				default:
					printf("Unknown flag: %s\n", arg);
					return 1;
			}
		} else {
			printf("Unknown flag: %s\n", arg);
				return 1;
		}
	}
}