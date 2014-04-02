/*
 * Copyright (c) 2013 The Board of Trustees of the University of Illinois,
 *                    Carnegie Mellon University.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT License.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the MIT License for more details.
 *
 * You should have received a copy of the MIT License along with this library;
 * if not, see http://opensource.org/licenses/MIT.
 *
 */
#include <estats/estats-int.h>

struct estats_error*
estats_connection_list_new(struct estats_connection_list** connection_list)
{
	estats_error* err = NULL;

	ErrIf(connection_list == NULL, ESTATS_ERR_INVAL);
	*connection_list = NULL;

	Chk(Malloc((void**) connection_list, sizeof(estats_connection_list)));
	memset((void*) *connection_list, 0, sizeof(estats_connection_list));
	list_head_init(&((*connection_list)->connection_head));
	list_head_init(&((*connection_list)->connection_info_head));

 Cleanup:
 	return err;
}

void
estats_connection_list_free(struct estats_connection_list** connection_list)
{
	struct estats_connection *conn, *tmp;
	estats_connection_info *conni, *tmpi;

	if (connection_list == NULL || *connection_list == NULL)
		return;

	list_for_each_safe(&((*connection_list)->connection_head), conn, tmp, list) {
		list_del(&conn->list);
		free(conn);
	}

	list_for_each_safe(&((*connection_list)->connection_info_head), conni, tmpi, list) {
		list_del(&conni->list);
		free(conni);
	}

	free(*connection_list);
	*connection_list = NULL;
}

struct estats_error*
estats_connection_info_new(struct estats_connection_info** connection_info)
{
	estats_error* err = NULL;

	ErrIf(connection_info == NULL, ESTATS_ERR_INVAL);

	*connection_info = NULL;
	
	Chk(Malloc((void**) connection_info, sizeof(estats_connection_info)));
	memset((void*) *connection_info, 0, sizeof(estats_connection_info));

 Cleanup:
 	return err;
}

void
estats_connection_info_free(struct estats_connection_info** connection_info)
{
	if (connection_info == NULL || *connection_info == NULL)
		return;

	free(*connection_info);
	*connection_info = NULL;
}

struct estats_error*
estats_connection_tuple_compare(int* res,
                               const struct estats_connection_tuple *s1,
                               const struct estats_connection_tuple *s2)
{
	estats_error* err = NULL;

	ErrIf(s1 == NULL || s2 == NULL, ESTATS_ERR_INVAL);

	*res = 1;

	if ( (s1->addr_type == s2->addr_type) &&
		(s1->rem_port == s2->rem_port) &&
        	strcmp((char *)(s1->rem_addr), (char *)(s2->rem_addr)) == 0 &&
         	(s1->local_port == s2->local_port) &&
         	strcmp((char *)(s1->local_addr), (char *)(s2->local_addr)) == 0 ) {

	*res = 0;
	}

Cleanup:
	return err;
}

struct estats_error*
estats_connection_tuple_as_strings(struct estats_connection_tuple_ascii* tuple_ascii, struct estats_connection_tuple* tuple)
{
	estats_error* err = NULL;

	ErrIf(tuple_ascii == NULL || tuple == NULL, ESTATS_ERR_INVAL);

	Chk(Sprintf(NULL, tuple_ascii->rem_port, "%u", tuple->rem_port));
	Chk(Sprintf(NULL, tuple_ascii->local_port, "%u", tuple->local_port));
	Chk(Sprintf(NULL, tuple_ascii->cid, "%d", tuple->cid));

	if (tuple->addr_type == ESTATS_ADDRTYPE_IPV4) {
        	Chk(Inet_ntop(AF_INET, (void*) (tuple->rem_addr), tuple_ascii->rem_addr, INET_ADDRSTRLEN));
        	Chk(Inet_ntop(AF_INET, (void*) (tuple->local_addr), tuple_ascii->local_addr, INET_ADDRSTRLEN));
	}
	else if (tuple->addr_type == ESTATS_ADDRTYPE_IPV6) {
        	Chk(Inet_ntop(AF_INET6, (void*) (tuple->rem_addr), tuple_ascii->rem_addr, INET6_ADDRSTRLEN));
        	Chk(Inet_ntop(AF_INET6, (void*) (tuple->local_addr), tuple_ascii->local_addr, INET6_ADDRSTRLEN));
	}
	else Err(ESTATS_ERR_ADDR_TYPE); 

Cleanup:
    return err;
}

estats_error*
estats_connection_info_get_cid(int* cid, const estats_connection_info* connection_info)
{
	estats_error* err = NULL;

	ErrIf(cid == NULL || connection_info == NULL, ESTATS_ERR_INVAL);
	*cid = connection_info->cid;

Cleanup:
	return err;
}
	
estats_error*
estats_connection_info_get_pid(int* pid, const estats_connection_info* connection_info)
{
	estats_error* err = NULL;

	ErrIf(pid == NULL || connection_info == NULL, ESTATS_ERR_INVAL);
	*pid = connection_info->pid;

Cleanup:
	return err;
}
	
estats_error*
estats_connection_info_get_uid(int* uid, const estats_connection_info* connection_info)
{
	estats_error* err = NULL;

	ErrIf(uid == NULL || connection_info == NULL, ESTATS_ERR_INVAL);
	*uid = connection_info->uid;

Cleanup:
	return err;
}

estats_error*
estats_connection_info_get_cmdline(char** str, const estats_connection_info* connection_info)
{
	estats_error* err = NULL;

	ErrIf(connection_info == NULL, ESTATS_ERR_INVAL);
	Chk(Strdup(str, connection_info->cmdline));

Cleanup:
	return err;
}

estats_error*
estats_connection_info_get_tuple(struct estats_connection_tuple* tuple, const estats_connection_info* connection_info)
{
	estats_error* err = NULL;

	ErrIf(tuple == NULL || connection_info == NULL, ESTATS_ERR_INVAL);

	*tuple = connection_info->tuple;

Cleanup:
	return err;
}

static struct estats_error* _estats_get_tcp_list(struct list_head*, const struct estats_connection_list*);
static struct estats_error* _estats_get_ino_list(struct list_head*);
static struct estats_error* _estats_get_pid_list(struct list_head*);

struct estats_error*
estats_connection_list_add_info(struct estats_connection_list* connection_list)
{
	estats_error* err = NULL;
	struct list_head* head;
	struct list_head tcp_head;
	struct list_head ino_head;
	struct list_head pid_head;
	estats_connection_info* tcp_info;
	estats_connection_info* ino_info;
	estats_connection_info* pid_info;
	estats_connection_info* conninfo;
	estats_connection_info* tmp;
	int dif;
	int tcp_entry, fd_entry;

	ErrIf(connection_list == NULL, ESTATS_ERR_INVAL);

	head = &connection_list->connection_info_head;

	Chk(_estats_get_tcp_list(&tcp_head, connection_list));
	Chk(_estats_get_ino_list(&ino_head));
	Chk(_estats_get_pid_list(&pid_head));

	estats_list_for_each(&tcp_head, tcp_info, list) {
	    tcp_entry = 0;
	    
	    estats_list_for_each(&ino_head, ino_info, list) {
		Chk(estats_connection_tuple_compare(&dif, &ino_info->tuple,
						    &tcp_info->tuple));
		if (!dif) {
		    tcp_entry = 1;
		    fd_entry = 0;

		    estats_list_for_each(&pid_head, pid_info, list) {
			if((ino_info->ino) &&
			    (pid_info->ino == ino_info->ino)) { //add entry 
			    fd_entry = 1;
			    
			    Chk(estats_connection_info_new(&conninfo));
			    
			    conninfo->pid = pid_info->pid;
			    strncpy(conninfo->cmdline, pid_info->cmdline,
				    sizeof(pid_info->cmdline));
			    conninfo->uid = ino_info->uid;
			    conninfo->state = ino_info->state;
			    conninfo->cid = tcp_info->cid;
			    conninfo->tuple = tcp_info->tuple;
			    conninfo->ino = ino_info->ino;

			    list_add_tail(head, &conninfo->list);
			}
		    }
		    
		    if(!fd_entry) { // add entry w/out cmdline
			Chk(estats_connection_info_new(&conninfo));

			conninfo->pid = -1;
			conninfo->uid = ino_info->uid;
			conninfo->state = ino_info->state;
			conninfo->cid = tcp_info->cid;
			conninfo->tuple = tcp_info->tuple;
			if (ino_info->ino) conninfo->ino = ino_info->ino;
			else conninfo->ino = -1;
			strncpy(conninfo->cmdline, "\0", 1);
			
			list_add_tail(head, &conninfo->list);
		    }
		}
	    }
	    
	    if(!tcp_entry) { // then connection has vanished;
			     // add residual cid info
	    Chk(estats_connection_info_new(&conninfo));

	    conninfo->cid = tcp_info->cid;
            conninfo->tuple = tcp_info->tuple;
	    conninfo->pid = -1;
            conninfo->uid = -1;
            conninfo->state = -1;
	    conninfo->ino = -1;

            strncpy(conninfo->cmdline, "\0", 1);

            list_add_tail(head, &conninfo->list);
	    }
	}

Cleanup:

	list_for_each_safe(&tcp_head, tcp_info, tmp, list) {
		list_del(&tcp_info->list);
		free(tcp_info);
	}

	list_for_each_safe(&ino_head, ino_info, tmp, list) {
		list_del(&ino_info->list);
		free(ino_info);
	}

	list_for_each_safe(&pid_head, pid_info, tmp, list) {
		list_del(&pid_info->list);
		free(pid_info);
	}
	
	return err;
}

static struct estats_error*
_estats_get_tcp_list(struct list_head* head, const estats_connection_list* connection_list)
{
	estats_error* err = NULL;
	const struct list_head* connection_head;
	struct estats_connection* conn;
	struct list_node* pos;
	int i;

	ErrIf(head == NULL || connection_list == NULL, ESTATS_ERR_INVAL);
	list_head_init(head);
	
	connection_head = &connection_list->connection_head;

	estats_list_for_each(connection_head, conn, list) {

		estats_connection_info* conninfo;
		Chk(estats_connection_info_new(&conninfo));

		conninfo->cid = conn->cid;
		for (i = 0; i < 16; i++)
			conninfo->tuple.rem_addr[i] = conn->rem_addr[i];
		for (i = 0; i < 16; i++)
			conninfo->tuple.local_addr[i] = conn->local_addr[i];
		conninfo->tuple.rem_port = conn->rem_port;
		conninfo->tuple.local_port = conn->local_port;
		conninfo->tuple.addr_type = conn->addr_type;

		list_add_tail(head, &conninfo->list);
	}

 Cleanup:
 	return err;
}

static struct estats_error*
_estats_get_ino_list(struct list_head* head)
{
	estats_error* err = NULL;
	FILE* file = NULL;
	FILE* file6 = NULL;
	char buf[256];
	int scan;
	struct in6_addr in6;

	ErrIf(head == NULL, ESTATS_ERR_INVAL);
	list_head_init(head);

	file = fopen("/proc/net/tcp", "r");
	file6 = fopen("/proc/net/tcp6", "r");

	if (file) {
		estats_connection_info* conninfo;

		while (fgets(buf, sizeof(buf), file) != NULL) {

			Chk(estats_connection_info_new(&conninfo));

			if ((scan = sscanf(buf,
				"%*u: %x:%hx %x:%hx %x %*x:%*x %*x:%*x %*x %u %*u %lu",
				(uint32_t *) &(conninfo->tuple.local_addr),
				(uint16_t *) &(conninfo->tuple.local_port),
				(uint32_t *) &(conninfo->tuple.rem_addr),
				(uint16_t *) &(conninfo->tuple.rem_port),
				(int *) &(conninfo->state),
				(uid_t *) &(conninfo->uid),
				(ino_t *) &(conninfo->ino)
				)) == 7) {

				conninfo->tuple.addr_type = ESTATS_ADDRTYPE_IPV4;
				list_add_tail(head, &conninfo->list);

			} else {
				estats_connection_info_free(&conninfo);
			}
		}
		fclose(file);
	}
	
    	if (file6) { 
		estats_connection_info* conninfo;
		char local_addr[INET6_ADDRSTRLEN];
		char rem_addr[INET6_ADDRSTRLEN];

		while (fgets(buf, sizeof(buf), file6) != NULL) {

			Chk(estats_connection_info_new(&conninfo));

			if ((scan = sscanf(buf,
				"%*u: %64[0-9A-Fa-f]:%hx %64[0-9A-Fa-f]:%hx %x %*x:%*x %*x:%*x %*x %u %*u %u", 
				(char *) &local_addr,
				(uint16_t *) &(conninfo->tuple.local_port),
				(char *) &rem_addr,
				(uint16_t *) &(conninfo->tuple.rem_port),
				(int *) &(conninfo->state),
				(uid_t *) &(conninfo->uid),
				(pid_t *) &(conninfo->ino)
				)) == 7) {
				
				sscanf(local_addr, "%8x%8x%8x%8x", &in6.s6_addr32[0], &in6.s6_addr32[1], &in6.s6_addr32[2], &in6.s6_addr32[3]); 

                		memcpy(&(conninfo->tuple.local_addr), &in6.s6_addr, 16);

				sscanf(rem_addr, "%8x%8x%8x%8x", &in6.s6_addr32[0], &in6.s6_addr32[1], &in6.s6_addr32[2], &in6.s6_addr32[3]);

                		memcpy(&(conninfo->tuple.rem_addr), &in6.s6_addr, 16);

				conninfo->tuple.addr_type = ESTATS_ADDRTYPE_IPV6;
				list_add_tail(head, &conninfo->list);

			} else {
				estats_connection_info_free(&conninfo);
	    		}
       		}
		fclose(file6);
	} 

 Cleanup:
 	return err;
}

static struct estats_error*
_estats_get_pid_list(struct list_head* head)
{
	estats_error* err = NULL;
	estats_connection_info* conninfo;
	DIR *dir, *fddir;
	struct dirent *direntp, *fddirentp;
	pid_t pid;
	char path[PATH_MAX];
	char buf[256];
	struct stat st;
	int stno;
	FILE* file;

	ErrIf(head == NULL, ESTATS_ERR_INVAL);
	list_head_init(head);

	Chk(Opendir(&dir, "/proc"));

	while ((direntp = readdir(dir)) != NULL) {
		if ((pid = atoi(direntp->d_name)) != 0) {

			sprintf(path, "%s/%d/%s/", "/proc", pid, "fd"); 

			if ((fddir = opendir(path)) != NULL) { //else lacks permissions 
	     
			while ((fddirentp = readdir(fddir)) != NULL) { 

		    	strcpy(buf, path);
		    	strcat(buf, fddirentp->d_name); 
		    	stno = stat(buf, &st); 

		    	if (S_ISSOCK(st.st_mode)) { // add new list entry

			sprintf(buf, "/proc/%d/status", pid);

			if ((file = fopen(buf, "r")) == NULL)
				continue;

			if (fgets(buf, sizeof(buf), file) == NULL)
			    	goto FileCleanup; 
       	
	                Chk(estats_connection_info_new(&conninfo));

			if (sscanf(buf, "Name: %16s\n",
				   conninfo->cmdline) != 1) {

			    estats_connection_info_free(&conninfo);
			    goto FileCleanup;
		       	}

			conninfo->ino = st.st_ino;
		       	conninfo->pid = pid;
                        list_add_tail(head, &conninfo->list);

 FileCleanup:
			fclose(file); 
		    	}
	       		}
	       		closedir(fddir);
	    		}
       		}
    	}
    	closedir(dir);

 Cleanup:
    return err;
}



