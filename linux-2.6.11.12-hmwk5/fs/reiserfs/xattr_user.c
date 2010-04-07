#include <linux/reiserfs_fs.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/xattr.h>
#include <linux/reiserfs_xattr.h>
#include <asm/uaccess.h>

#ifdef CONFIG_REISERFS_FS_POSIX_ACL
# include <linux/reiserfs_acl.h>
#endif

#define XATTR_USER_PREFIX "user."

static int
user_get (struct inode *inode, const char *name, void *buffer, size_t size)
{

    int error;

    if (strlen(name) < sizeof(XATTR_USER_PREFIX))
        return -EINVAL;

    if (!reiserfs_xattrs_user (inode->i_sb))
        return -EOPNOTSUPP;

    error = reiserfs_permission_locked (inode, MAY_READ, NULL);
    if (error)
        return error;

    return reiserfs_xattr_get (inode, name, buffer, size);
}

static int
user_set (struct inode *inode, const char *name, const void *buffer,
          size_t size, int flags)
{

    int error;

    if (strlen(name) < sizeof(XATTR_USER_PREFIX))
        return -EINVAL;

    if (!reiserfs_xattrs_user (inode->i_sb))
        return -EOPNOTSUPP;

    if (!S_ISREG (inode->i_mode) &&
        (!S_ISDIR (inode->i_mode) || inode->i_mode & S_ISVTX))
        return -EPERM;

    error = reiserfs_permission_locked (inode, MAY_WRITE, NULL);
    if (error)
        return error;

    return reiserfs_xattr_set (inode, name, buffer, size, flags);
}

static int
user_del (struct inode *inode, const char *name)
{
    int error;

    if (strlen(name) < sizeof(XATTR_USER_PREFIX))
        return -EINVAL;

    if (!reiserfs_xattrs_user (inode->i_sb))
        return -EOPNOTSUPP;

    if (!S_ISREG (inode->i_mode) &&
        (!S_ISDIR (inode->i_mode) || inode->i_mode & S_ISVTX))
        return -EPERM;

    error = reiserfs_permission_locked (inode, MAY_WRITE, NULL);
    if (error)
        return error;

    return 0;
}

static int
user_list (struct inode *inode, const char *name, int namelen, char *out)
{
    int len = namelen;
    if (!reiserfs_xattrs_user (inode->i_sb))
        return 0;

    if (out)
        memcpy (out, name, len);

    return len;
}

struct reiserfs_xattr_handler user_handler = {
	.prefix = XATTR_USER_PREFIX,
	.get = user_get,
	.set = user_set,
	.del = user_del,
	.list = user_list,
};
