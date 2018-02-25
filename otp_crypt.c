#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

void
usage()
{
	fprintf(stderr, "\nUsage: \n\n"
			"Encryption:\n"
			"otp_crypt -e Input_File -k Output_Key "
			"-o Encrypted_File\n\n"
			"Decryption:\n"
			"otp_crypt -d Encrypted_File -k Input_Key "
			"-o Decrypted_File\n\n"
			);
}

void
encrypt(unsigned char *path,
	unsigned char *key_path,
	unsigned char *crypt_path)
{
	int fd_path = open(path, O_RDONLY);
	int fd_rand = open("/dev/random", O_RDONLY);
	int fd_key = open(key_path, O_WRONLY | O_CREAT, S_IRWXU);
	int fd_crypt = open(crypt_path, O_WRONLY | O_CREAT, S_IRWXU);
	unsigned char *buffer = NULL;
	unsigned char *key = NULL;
	unsigned long long i = 0;
	unsigned long long size;
	struct stat *stats = malloc(sizeof(struct stat));

	stat(path, stats);

	size = stats->st_size;

	buffer = malloc(size);
	key = malloc(size);

	read(fd_path, buffer, size);
	read(fd_rand, key, size);
	for (i = 0; i < size; i++)
		buffer[i] = buffer[i] ^ key[i];

	write(fd_key, key, size);
	write(fd_crypt, buffer, size);

	close(fd_path);
	close(fd_rand);
	close(fd_key);
	close(fd_crypt);
	free(buffer);
	free(key);
	free(stats);
}

void
decrypt(unsigned char *crypt_path,
	unsigned char *key_path,
	unsigned char *decrypt_path)
{
	int fd_path = open(crypt_path, O_RDONLY);
	int fd_key = open(key_path, O_RDONLY);
	int fd_decrypt = open(decrypt_path, O_WRONLY | O_CREAT, S_IRWXU);
	unsigned char *buffer = NULL;
	unsigned char *key = NULL;
	unsigned long long i = 0;
	unsigned long long size;
	struct stat *stats = malloc(sizeof(struct stat));

	stat(crypt_path, stats);

	size = stats->st_size;

	buffer = malloc(size);
	key = malloc(size);

	read(fd_path, buffer, size);
	read(fd_key, key, size);

	for (i = 0; i < size; i++)
		buffer[i] = buffer[i] ^ key[i];

	write(fd_decrypt, buffer, size);

	close(fd_path);
	close(fd_key);
	close(fd_decrypt);
	free(buffer);
	free(key);
	free(stats);
}

int main(int argc, char *argv[])
{
	int c, index, ind = 0;
	unsigned int sete = 0, setd = 0;
	unsigned char *input = NULL;
	unsigned char *key = NULL;
	unsigned char *output = NULL;

	while ((c = getopt(argc, argv, "d:e:k:o:")) != -1) {
		switch (c) {
			case 'd':	input = optarg;
					setd++;
					break;
			case 'e':	input = optarg;
					sete++;
        				break;
        		case 'k':	key = optarg;
        				break;
        		case 'o':	output = optarg;
        				break;
			default: 	usage();
					return 1;
        	}
    	}
    	for (index = optind; index < argc; index++)
		fprintf(stderr, "Non-option argument %s\n", argv[index]);
	if (sete && setd) {
		fprintf(stderr, "You cannot specify both 'encrypt' and 'decrypt'\n");
		usage();
		return 1;
	}
	if (input == NULL) {
		fprintf(stderr, "No Input specified\n");
		usage();
		return 1;
	}
	if (key == NULL) {
		fprintf(stderr, "No Key specified\n");
		usage();
		return 1;
	}
	if (output == NULL) {
		fprintf(stderr, "No Output specified\n");
		usage();
		return 1;
	}
	if (sete)
		encrypt(input, key, output);
	if (setd)
		decrypt(input, key, output);
	return 0;
}
