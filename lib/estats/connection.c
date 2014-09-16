/*
 * Copyright (c) 2014 The Board of Trustees of the University of Illinois,
 *		      Carnegie Mellon University.
 *
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
/* hash functions extracted from 
 * lookup3.c, by Bob Jenkins, May 2006, Public Domain.
 */
/* hash table function provided by uthash by
 * Copyright (c) 2003-2014, Troy D. Hanson http://troydhanson.github.com/uthash/
 * All rights reserved.
 *
 */

#include <estats/estats-int.h>

// forward declaration of some private functions
static struct estats_error* _estats_get_tcp_list(const struct estats_connection_list*,
						 struct s_tcp_hash**);

static struct estats_error* _estats_get_ino_list(struct s_ino_hash**);

static struct estats_error* _estats_get_pid_list(struct s_pid_hash**);


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
estats_connection_vars_list_new(struct estats_connection_vars_list** connection_vars_list)
{
	estats_error* err = NULL;

	ErrIf(connection_vars_list == NULL, ESTATS_ERR_INVAL);
	*connection_vars_list = NULL;

	Chk(Malloc((void**) connection_vars_list, sizeof(estats_connection_vars_list)));
	memset((void*) *connection_vars_list, 0, sizeof(estats_connection_vars_list));
	list_head_init(&((*connection_vars_list)->connection_vars_head));
	list_head_init(&((*connection_vars_list)->connection_info_head));

 Cleanup:
	return err;
}

void
estats_connection_vars_list_free(struct estats_connection_vars_list** connection_vars_list)
{
	struct estats_connection_vars *conn, *tmp;
	estats_connection_info *conni, *tmpi;

	if (connection_vars_list == NULL || *connection_vars_list == NULL)
		return;

	list_for_each_safe(&((*connection_vars_list)->connection_vars_head), conn, tmp, list) {
		list_del(&conn->list);
		estats_connection_vars_free(&conn);
	}

	list_for_each_safe(&((*connection_vars_list)->connection_info_head), conni, tmpi, list) {
		list_del(&conni->list);
		free(conni);
	}

	free(*connection_vars_list);
	*connection_vars_list = NULL;
}

struct estats_error*
estats_connection_vars_new(struct estats_connection_vars** connection_vars)
{
	estats_error* err = NULL;
	estats_val_data *data = NULL;

	ErrIf(connection_vars == NULL, ESTATS_ERR_INVAL);

	*connection_vars = NULL;

	Chk(estats_val_data_new(&data));

	Chk(Malloc((void**) connection_vars, sizeof(estats_connection_vars)));
	memset((void*) *connection_vars, 0, sizeof(estats_connection_vars));

	(*connection_vars)->data = data;

	return err;

 Cleanup:
	if (data)
		estats_val_data_free(&data);
	return err;
}

void
estats_connection_vars_free(struct estats_connection_vars** connection_vars)
{
	if (connection_vars == NULL || *connection_vars == NULL)
		return;

	estats_val_data_free(&(*connection_vars)->data);
	free(*connection_vars);
	*connection_vars = NULL;
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
estats_connection_tuple_as_strings(struct estats_connection_tuple_ascii* tuple_ascii, 
				   struct estats_connection_tuple* tuple)
{
	estats_error* err = NULL;
	
	ErrIf(tuple_ascii == NULL || tuple == NULL, ESTATS_ERR_INVAL);
	
	Chk(Sprintf(NULL, tuple_ascii->rem_port, "%u", tuple->rem_port));
	Chk(Sprintf(NULL, tuple_ascii->local_port, "%u", tuple->local_port));
	Chk(Sprintf(NULL, tuple_ascii->cid, "%d", tuple->cid));
	
	if (tuple->addr_type == ESTATS_ADDRTYPE_IPV4) {
		Chk(Inet_ntop(AF_INET, (void*) (tuple->rem_addr), 
			      tuple_ascii->rem_addr, INET_ADDRSTRLEN));
		Chk(Inet_ntop(AF_INET, (void*) (tuple->local_addr), 
			      tuple_ascii->local_addr, INET_ADDRSTRLEN));
	}
	else if (tuple->addr_type == ESTATS_ADDRTYPE_IPV6) {
		Chk(Inet_ntop(AF_INET6, (void*) (tuple->rem_addr), 
			      tuple_ascii->rem_addr, INET6_ADDRSTRLEN));
		Chk(Inet_ntop(AF_INET6, (void*) (tuple->local_addr), 
			      tuple_ascii->local_addr, INET6_ADDRSTRLEN));
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
estats_connection_info_get_tuple(struct estats_connection_tuple* tuple, 
				 const estats_connection_info* connection_info)
{
	estats_error* err = NULL;

	ErrIf(tuple == NULL || connection_info == NULL, ESTATS_ERR_INVAL);

	*tuple = connection_info->tuple;

Cleanup:
	return err;
}


/* create a series of hashes and compare the overlapping information
 * to extract extra information like the command line, PID and UID */
struct estats_error*
estats_connection_list_add_info(struct estats_connection_list* connection_list)
{
	estats_error* err = NULL;
	struct list_head* head;
	estats_connection_info* conninfo;
	estats_connection_info* tmp_ptr;	
	int myino = 0;

	uint32_t b, c;

	struct s_tcp_hash *tcp_hash = NULL; // the main hash table
	struct s_tcp_hash *tcp_hash_tmp = NULL; // the current pointer into the hash
	struct s_tcp_hash *tcp_hash_del = NULL; // tmp pointer to the next pointed in the hash
	struct s_ino_hash *ino_hash = NULL;
	struct s_ino_hash *ino_hash_tmp = NULL;
	struct s_ino_hash *ino_hash_del = NULL;
	struct s_pid_hash *pid_hash = NULL;
	struct s_pid_hash *pid_hash_tmp;
	struct s_pid_hash *pid_hash_del = NULL;
	struct estats_connection_tuple_ascii tuple_ascii = { 0 };	

	ErrIf(connection_list == NULL, ESTATS_ERR_INVAL);

	head = &connection_list->connection_info_head;

	// the full list of all the necessary network data and return a hash of the data. 
	Chk(_estats_get_tcp_list(connection_list, &tcp_hash));

	Chk(_estats_get_ino_list(&ino_hash));

	Chk(_estats_get_pid_list(&pid_hash));

	// iterate over the tcp connections looking for a match in the keys of the
	// ino hash. for each match we want to get the necessary information from the pid hash
	
	for (tcp_hash_tmp = tcp_hash; tcp_hash_tmp != NULL; tcp_hash_tmp=tcp_hash_tmp->hh.next) {
		HASH_FIND_INT(ino_hash, &tcp_hash_tmp->tcpkey, ino_hash_tmp);
		if (ino_hash_tmp != NULL) {
			// the tuples match now check to see if the ino numbers match
			myino = ino_hash_tmp->ino;
			HASH_FIND_INT(pid_hash, &myino, pid_hash_tmp);
			if (pid_hash_tmp != NULL) {
				// a corresponding inode was found in the process table
				// so we can now grab the command line
				Chk(estats_connection_info_new(&conninfo));
				conninfo->pid = pid_hash_tmp->info->pid;
				strncpy(conninfo->cmdline, pid_hash_tmp->info->cmdline,
					sizeof(pid_hash_tmp->info->cmdline));
				conninfo->uid = ino_hash_tmp->info->uid;
				conninfo->state = ino_hash_tmp->info->state;
				conninfo->cid = tcp_hash_tmp->info->cid;
				conninfo->tuple = tcp_hash_tmp->info->tuple;
				conninfo->ino = ino_hash_tmp->info->ino;
				list_add_tail(head, &conninfo->list);
			} else {
				// we did not find the inode in the process table
				// so we leave the command line blank
				Chk(estats_connection_info_new(&conninfo));
				conninfo->pid = -1;
				conninfo->uid = ino_hash_tmp->info->uid;
				conninfo->state = ino_hash_tmp->info->state;
				conninfo->cid = tcp_hash_tmp->info->cid;
				conninfo->tuple = tcp_hash_tmp->info->tuple;
				if (ino_hash_tmp->info->ino) 
					conninfo->ino = ino_hash_tmp->info->ino;
				else 
					conninfo->ino = -1;
				strncpy(conninfo->cmdline, "\0", 1);
				list_add_tail(head, &conninfo->list);
			}
		} else {
			// no match for the tuples so the connection vanished
			// add the residual cid information and be done with it
			Chk(estats_connection_info_new(&conninfo));
			conninfo->cid = tcp_hash_tmp->info->cid;
			conninfo->tuple = tcp_hash_tmp->info->tuple;
			conninfo->pid = -1;
			conninfo->uid = -1;
			conninfo->state = -1;
			conninfo->ino = -1;
			strncpy(conninfo->cmdline, "\0", 1);
			list_add_tail(head, &conninfo->list);
		}
	}

Cleanup:

	/* we have to free all of the hashes now */
	HASH_ITER(hh, pid_hash, pid_hash_tmp, pid_hash_del) {
		HASH_DEL(pid_hash, pid_hash_tmp);  /* delete and advance to next */
		free(pid_hash_tmp->info);
		free(pid_hash_tmp);	       
	}

	HASH_ITER(hh, ino_hash, ino_hash_tmp, ino_hash_del) {
		HASH_DEL(ino_hash, ino_hash_tmp);  
		free(ino_hash_tmp->info);
		free(ino_hash_tmp);	       
	}

	HASH_ITER(hh, tcp_hash, tcp_hash_tmp, tcp_hash_del) {
		HASH_DEL(tcp_hash, tcp_hash_tmp);  
		free(tcp_hash_tmp->info);
		free(tcp_hash_tmp);	       
	}

	return err;
}

// get a full list of all of our tcp connections
static struct estats_error*
_estats_get_tcp_list(const estats_connection_list* connection_list, struct s_tcp_hash **tcp_hash )
{
	estats_error* err = NULL;
	const struct list_head* connection_head;
	struct estats_connection* conn;
	struct list_node* pos;
	struct s_tcp_hash* tcp_hash_tmp;
	struct s_tcp_hash* tcp_hash_chk;
	int i;
	uint32_t c;

	ErrIf(connection_list == NULL, ESTATS_ERR_INVAL);
	
	connection_head = &connection_list->connection_head;

	estats_list_for_each(connection_head, conn, list) {

		tcp_hash_tmp = (struct s_tcp_hash*)malloc(sizeof(struct s_tcp_hash));
		Chk(estats_connection_info_new(&tcp_hash_tmp->info));

		tcp_hash_tmp->info->cid = conn->cid;
		
		for (i = 0; i < 16; i++)
			tcp_hash_tmp->info->tuple.rem_addr[i] = conn->rem_addr[i];
		for (i = 0; i < 16; i++)
			tcp_hash_tmp->info->tuple.local_addr[i] = conn->local_addr[i];
		tcp_hash_tmp->info->tuple.rem_port = conn->rem_port;
		tcp_hash_tmp->info->tuple.local_port = conn->local_port;
		tcp_hash_tmp->info->tuple.addr_type = conn->addr_type;


		// create a unique non-cryptographic hash of the tuple information
		c = hashlittle(&tcp_hash_tmp->info->tuple, sizeof(tcp_hash_tmp->info->tuple), 0); 

		// add information to our hash table here (depends on uthash.h)
		HASH_FIND_INT(*tcp_hash, &c, tcp_hash_chk); // see if the entry already exists
		if (tcp_hash_chk == NULL) { // if not then create it and add the data
			tcp_hash_tmp->tcpkey = c;
			HASH_ADD_INT(*tcp_hash, tcpkey, tcp_hash_tmp);
		} else {
			// an entry with this tuple hash exists
			// which makes me feel like i'm taking crazy pill
			// so free the structs! FREE THEM!!!
			free(tcp_hash_tmp->info);
			free(tcp_hash_tmp);
		}

	}


 Cleanup:
	return err;
}

static struct estats_error*
_estats_get_ino_list(struct s_ino_hash** ino_hash)
{
	estats_error* err = NULL;
	FILE* file = NULL;
	FILE* file6 = NULL;
	char buf[256];
	int scan;
	struct in6_addr in6;
	struct s_ino_hash* ino_hash_tmp;
	struct s_ino_hash* ino_hash_chk;
	uint32_t c;
	
	file = fopen("/proc/net/tcp", "r");
	file6 = fopen("/proc/net/tcp6", "r");

	if (file) {

		while (fgets(buf, sizeof(buf), file) != NULL) {

			ino_hash_tmp = (struct s_ino_hash*)malloc(sizeof(struct s_ino_hash));
			Chk(estats_connection_info_new(&ino_hash_tmp->info));

			if ((scan = sscanf(buf,
				"%*u: %x:%hx %x:%hx %x %*x:%*x %*x:%*x %*x %u %*u %lu",
					   (uint32_t *) &(ino_hash_tmp->info->tuple.local_addr),
					   (uint16_t *) &(ino_hash_tmp->info->tuple.local_port),
					   (uint32_t *) &(ino_hash_tmp->info->tuple.rem_addr),
					   (uint16_t *) &(ino_hash_tmp->info->tuple.rem_port),
					   (int *) &(ino_hash_tmp->info->state),
				(uid_t *) &(ino_hash_tmp->info->uid),
				(ino_t *) &(ino_hash_tmp->info->ino)
				)) == 7) {

				ino_hash_tmp->info->tuple.addr_type = ESTATS_ADDRTYPE_IPV4;
				
				// see _estats_get_tcp_list for comments on the following line
				c = hashlittle(&ino_hash_tmp->info->tuple, sizeof(ino_hash_tmp->info->tuple), 0);
				
				HASH_FIND_INT(*ino_hash, &c, ino_hash_chk);
				if (ino_hash_chk==NULL) {
					ino_hash_tmp->inokey = c;
					ino_hash_tmp->ino = ino_hash_tmp->info->ino;
					HASH_ADD_INT(*ino_hash, inokey, ino_hash_tmp);
				} else {
					free(ino_hash_tmp->info);
					free(ino_hash_tmp);
				}
			} else {
				free(ino_hash_tmp->info);
				free(ino_hash_tmp);				
			}
		}
		fclose(file);
	}
	
	if (file6) { 
		char local_addr[INET6_ADDRSTRLEN];
		char rem_addr[INET6_ADDRSTRLEN];

		while (fgets(buf, sizeof(buf), file6) != NULL) {

			ino_hash_tmp = (struct s_ino_hash*)malloc(sizeof(struct s_ino_hash));
			Chk(estats_connection_info_new(&ino_hash_tmp->info));

			if ((scan = sscanf(buf,
				"%*u: %64[0-9A-Fa-f]:%hx %64[0-9A-Fa-f]:%hx %x %*x:%*x %*x:%*x %*x %u %*u %u", 
				(char *) &local_addr,
				(uint16_t *) &(ino_hash_tmp->info->tuple.local_port),
				(char *) &rem_addr,
				(uint16_t *) &(ino_hash_tmp->info->tuple.rem_port),
				(int *) &(ino_hash_tmp->info->state),
				(uid_t *) &(ino_hash_tmp->info->uid),
				(pid_t *) &(ino_hash_tmp->info->ino)
				)) == 7) {
				
				sscanf(local_addr, "%8x%8x%8x%8x", &in6.s6_addr32[0], 
				       &in6.s6_addr32[1], &in6.s6_addr32[2], &in6.s6_addr32[3]); 
				memcpy(&(ino_hash_tmp->info->tuple.local_addr), &in6.s6_addr, 16);
				sscanf(rem_addr, "%8x%8x%8x%8x", &in6.s6_addr32[0], 
				       &in6.s6_addr32[1], &in6.s6_addr32[2], &in6.s6_addr32[3]);
				memcpy(&(ino_hash_tmp->info->tuple.rem_addr), &in6.s6_addr, 16);
				ino_hash_tmp->info->tuple.addr_type = ESTATS_ADDRTYPE_IPV6;

				c = hashlittle(&ino_hash_tmp->info->tuple, sizeof(ino_hash_tmp->info->tuple), 0);
				
				// see _estats_get_tcp_list for comments on the following line
				HASH_FIND_INT(*ino_hash, &c, ino_hash_chk);
				if (ino_hash_chk==NULL) {
					ino_hash_tmp->inokey = c;
					ino_hash_tmp->ino = ino_hash_tmp->info->ino;
					HASH_ADD_INT(*ino_hash, inokey, ino_hash_tmp);
				} else {
					free(ino_hash_tmp->info);
					free(ino_hash_tmp);
				}
			} else {
				free(ino_hash_tmp->info);
				free(ino_hash_tmp);
			} 
		}
		fclose(file6);
	} 

 Cleanup:
	return err;
}

static struct estats_error*
_estats_get_pid_list(struct s_pid_hash **pid_hash)
{
	estats_error* err = NULL;
	DIR *dir, *fddir;
	struct dirent *direntp, *fddirentp;
	pid_t pid;
	char path[PATH_MAX];
	char buf[256];
	struct stat st;
	int stno;
	FILE* file;
	struct s_pid_hash* pid_hash_tmp = NULL;
	struct s_pid_hash* pid_hash_chk = NULL;
	uint32_t c;
	char tmpcmdline[16] = "\0";

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
						
						// we do this so we don't open status multiple 
						// times if a applications has multiple sockets. 
						if (!tmpcmdline[0]) {
							sprintf(buf, "/proc/%d/status", pid);
							if ((file = fopen(buf, "r")) == NULL)
								continue;
							if (fgets(buf, sizeof(buf), file) == NULL){
								continue;
								fclose(file); 
							}
							if (sscanf(buf, "Name: %16s\n", tmpcmdline) != 1) {
								fclose(file); 
								continue;
							}
							fclose(file);
						}
						HASH_FIND_INT(*pid_hash, &st.st_ino, pid_hash_tmp);
						if (pid_hash_tmp == NULL) {
							pid_hash_tmp = (struct s_pid_hash*)malloc(sizeof(struct s_pid_hash));
							Chk(estats_connection_info_new(&pid_hash_tmp->info));
							strcpy(pid_hash_tmp->info->cmdline, tmpcmdline);
							pid_hash_tmp->info->ino = st.st_ino;
							pid_hash_tmp->info->pid = pid;
							pid_hash_tmp->ino = st.st_ino;
							HASH_ADD_INT(*pid_hash, ino, pid_hash_tmp);
						} 
					}
				}
				closedir(fddir);
				tmpcmdline[0] = '\0';
			}
		}
	}
	closedir(dir);
Cleanup:
	return err;
}
