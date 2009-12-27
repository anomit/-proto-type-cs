#define FUSE_USE_VERSION 26
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
//#include <dns.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PRIV_DATA_SIZE 256

static struct private_data_t {
    char data[256];
};


#define PRIV_DATA ((struct private_data_t *) fuse_get_context()->private_data)

static const char *basic_path = "/cs";

static int basic_getattr(const char *path, struct stat *stbuf)
{
    int res = 0;
    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    }
    else if (strcmp(path, basic_path) == 0) {
        stbuf->st_mode = S_IFREG | 0644;
        stbuf->st_nlink = 1;
        stbuf->st_size = strlen(PRIV_DATA->data);
    }
    else
        res = -ENOENT;
    return res;
}

static int basic_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
        off_t offset, struct fuse_file_info *fi)
{
    (void)offset;
    (void)fi;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, "cs", NULL, 0);

    return 0;
}

static int basic_open(const char *path, struct fuse_file_info *fi)
{
    if (strcmp(path, basic_path) != 0)
        return -ENOENT;
    //if ((fi->flags & 3) != O_RDONLY)
    //    return -EACCES;

    return 0;
}

static int basic_write(const char *path, const char *buf, size_t size,
        off_t offset, struct fuse_file_info *fi)
{
    const char *clone_path = "/net/tcp/clone ";

    if (strcmp(path, basic_path) != 0)
        return -ENOENT;

    //strncpy(PRIV_DATA->data, buf, size);
    memset(PRIV_DATA->data, '\0', PRIV_DATA_SIZE);
    strncpy(PRIV_DATA->data, clone_path, strlen(clone_path));

    struct hostent *query_res = gethostbyname(buf);
    struct in_addr **addr_list = (struct in_addr **)query_res->h_addr_list;
    char *addr1 = inet_ntoa(*addr_list[0]);
    strncat(PRIV_DATA->data, addr1, strlen(addr1));

    /*
    int i =0;
    stralloc out = {0};
    stralloc fqdn = {0};

    stralloc_ready(&out, PRIV_DATA_SIZE);
    stralloc_ready(&fqdn, size);

    stralloc_copyb(&fqdn, buf, size);

    dns_ip4(&out, &fqdn);
    int query_res_offset = strlen(clone_path);

    while (i < out.len)
    {
        PRIV_DATA->data[i + query_res_offset] = out.s + i;
        ++i;
    }
    */
    return size;
}

static int basic_read(const char *path, char *buf, size_t size,
        off_t offset, struct fuse_file_info *fi)
{
    if (strcmp(path, basic_path) != 0)
        return -ENOENT;

    size_t len = strlen(PRIV_DATA->data);
    if (offset < len) {
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, PRIV_DATA->data, size);
    }
    else
        size = 0;
    return size;
}

struct fuse_operations basic_opers = {
    .open = basic_open,
    .readdir = basic_readdir,
    .getattr = basic_getattr,
    .read = basic_read,
    .write = basic_write
};
int main(int argc, char *argv[])
{
    struct private_data_t *private_data;
    private_data = calloc(sizeof(struct private_data_t), 1);
    memset(private_data->data, '\0', PRIV_DATA_SIZE);
    return fuse_main(argc, argv, &basic_opers, private_data);
}
