#include "common.h"
#include "parse.h"
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>


int output_file(int fd, struct dbheader_t *dbhdr) {
	if (fd < 0) {
		printf("Bad FD from the user");
		return STATUS_ERROR;
	}
	dbhdr->magic = htonl(dbhdr->magic);
	dbhdr->filesize = htonl(dbhdr->filesize);
	dbhdr->count = htons(dbhdr->count);
	dbhdr->version = htons(dbhdr->version);

	lseek(fd, 0, SEEK_SET);
	write(fd, dbhdr, sizeof(struct dbheader_t));

}


int create_db_header(int fd, struct dbheader_t **headerOut) {
    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if (header == NULL) {
	printf("Calloc failed to create db header\n");
	return STATUS_ERROR;
    }
    header->version = 0x1;
    header->count = 0;
    header->magic = HEADER_MAGIC;
    header->filesize = sizeof(struct dbheader_t);
    *headerOut = header;

    return STATUS_SUCESS;
}


int validate_db_header(int fd, struct dbheader_t **headerOut) {
	if (fd < 0) {
		printf("Bad FD from the user");
		return STATUS_ERROR;
	}

	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
	if (header == NULL) {
		printf("Calloc error");
		return STATUS_ERROR;
	}

	if (read(fd, header, sizeof(struct dbheader_t)) !=  sizeof(struct dbheader_t)) {
		perror("read");
		free(header);
		return STATUS_ERROR;
	}
	header->version = ntohs(header->version);
	header->count = ntohs(header->count);
	header->magic = ntohl(header->magic);
	header->filesize= ntohl(header->filesize);

	if (header->version) {
		printf("Improper header version\n");
		free(header);
		return -1;
	}
		if (header->count) {
		printf("Improper header magic\n");
		free(header);
		return -1;
	}
	struct stat dbstat = {0};
	fstat(fd, &dbstat);

	if (header->filesize != dbstat.st_size) {
		printf("Corrupted database");
		free(header);
		return -1;
	}
	*headerOut = header;
}



int read_employees(int fd, struct dbheader_t *, struct employee_t **employeesOut);
