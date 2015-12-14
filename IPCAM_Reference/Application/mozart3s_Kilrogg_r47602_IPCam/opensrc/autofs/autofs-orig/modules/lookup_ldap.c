#ident "$Id: lookup_ldap.c,v 1.2 2000/06/19 23:57:06 hpa Exp $"
/*
 * lookup_ldap.c
 *
 * Module for Linux automountd to access automount maps in LDAP directories.
 *
 */

#include <sys/types.h>
#include <ctype.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <lber.h>
#include <ldap.h>

#define MODULE_LOOKUP
#include "automount.h"

#define MAPFMT_DEFAULT "sun"

#define MODPREFIX "lookup(ldap): "

#define OBJECTCLASS "automount"
#define ATTRIBUTE "automountInformation"
#define WILDCARD "/"

struct lookup_context {
  char *server, *base;
  struct parse_mod *parser;
};

int lookup_version = AUTOFS_LOOKUP_VERSION; /* Required by protocol */

/*
 * This initializes a context (persistent non-global data) for queries to
 * this module.  Return zero if we succeed.
 */
int lookup_init(const char *mapfmt, int argc, const char * const *argv,
    void **context)
{
  struct lookup_context *ctxt = NULL;
  int rv, l;
  LDAP *ldap;

  /* If we can't build a context, bail. */
  ctxt = (struct lookup_context*) malloc(sizeof(struct lookup_context));
  *context = ctxt;
  if( ctxt == NULL ) {
    syslog(LOG_INFO, MODPREFIX "malloc: %m");
    return 1;
  }
  memset(ctxt, 0, sizeof(struct lookup_context));

  /* If a map type isn't explicitly given, parse it like sun entries. */
  if( mapfmt == NULL ) {
    mapfmt = MAPFMT_DEFAULT;
  }

  /* Now we sanity-check by binding to the server temporarily.  We have to be
   * a little strange in here, because we want to provide for use of the
   * "default" server, which is set in an ldap.conf file somewhere. */
  if(strchr(argv[0], ':') != NULL) {
    l = strchr(argv[0], ':') - argv[0];
    /* Isolate the server's name. */
    ctxt->server = malloc(l + 1);
    memset(ctxt->server, 0, l + 1);
    memcpy(ctxt->server, argv[0], l);
    /* Isolate the base DN. */
    ctxt->base = malloc(strlen(argv[0]) - l);
    memset(ctxt->base, 0, strlen(argv[0]) - l);
    memcpy(ctxt->base, argv[0] + l + 1, strlen(argv[0]) - l - 1);
  } else {
    /* Use the default server;  isolate the base DN's name. */
    l = strlen(argv[0]);
    ctxt->server = NULL;
    ctxt->base = malloc(l + 1);
    memset(ctxt->base, 0, l + 1);
    memcpy(ctxt->base, argv[0], l);
  }

  syslog(LOG_DEBUG, MODPREFIX "server = \"%s\", base dn = \"%s\"",
         ctxt->server ? ctxt->server : "(default)", ctxt->base);

  /* Initialize the LDAP context. */
  if( ( ldap = ldap_init(ctxt->server, LDAP_PORT)) == NULL ) {
    syslog(LOG_CRIT, MODPREFIX "couldn't initialize LDAP");
    return 1;
  }

  /* Connect to the server as an anonymous user. */
  rv = ldap_simple_bind_s(ldap, ctxt->base, NULL);
  if( rv != LDAP_SUCCESS ) {
    syslog(LOG_CRIT, MODPREFIX "couldn't connect to %s", ctxt->server);
    return 1;
  }

  /* Okay, we're done here. */
  ldap_unbind(ldap);

  /* Open the parser, if we can. */
  return !(ctxt->parser = open_parse(mapfmt, MODPREFIX, argc - 1, argv + 1));
}

/* Lookup by key and pass a filesystem to a parser. */
int lookup_mount(const char *root, const char *name, int name_len, void *context)
{
  struct lookup_context *ctxt = (struct lookup_context *) context;
  int rv, i, l;
  char *query;
  LDAPMessage *result, *e;
  char **values;
  char *attrs[] = {ATTRIBUTE, NULL};
  LDAP *ldap;

  chdir("/");  /* If this is not here the filesystem stays
         busy, for some reason... */

  if( ctxt == NULL ) {
    syslog(LOG_CRIT, MODPREFIX "context was NULL");
    return 0;
  }

  /* Build a query string. */
  l = name_len + strlen("(&(objectclass=" OBJECTCLASS ")(cn=))") + 2;

  query = malloc(l);
  if( query == NULL ) {
    syslog(LOG_INFO, MODPREFIX "malloc: %m");
    return 0;
  }

  memset(query, '\0', l);
  if( sprintf(query, "(&(objectclass=" OBJECTCLASS ")(cn=%s))", name) >= l ) {
    syslog(LOG_DEBUG, MODPREFIX "error forming query string");
  }
  query[l - 1] = '\0';

  /* Initialize the LDAP context. */
  if( (ldap = ldap_init(ctxt->server, LDAP_PORT) ) == NULL ) {
    syslog(LOG_CRIT, MODPREFIX "couldn't initialize LDAP connection"
           " to %s", ctxt->server ? ctxt->server : "default server");
    free(query);
    return 1;
  }

  /* Connect to the server as an anonymous user. */
  rv = ldap_simple_bind_s(ldap, ctxt->base, NULL);
  if ( rv != LDAP_SUCCESS ) {
    syslog(LOG_CRIT, MODPREFIX "couldn't bind to %s",
           ctxt->server ? ctxt->server : "default server");
    free(query);
    return 1;
  }

  /* Look around. */
  syslog(LOG_DEBUG, MODPREFIX "searching for \"%s\"", query);
  rv = ldap_search_s(ldap, ctxt->base, LDAP_SCOPE_SUBTREE,
                     query, attrs, 0, &result);

  if( ( rv != LDAP_SUCCESS) || ( result == NULL ) ) {
    syslog(LOG_INFO, MODPREFIX "query failed for %s", query);
    free(query);
    return 1;
  }

  e = ldap_first_entry(ldap, result);

  /* If we got no answers, try it with "/" instead, which makes a better
   * wildcard thatn "*" for LDAP, and also happens to be illegal for actual
   * directory names. */
  if( e == NULL ) {
    syslog(LOG_DEBUG, MODPREFIX "no entry for \"%s\" found, trying cn=\"/\"",
           name);

    sprintf(query, "(&(objectclass=" OBJECTCLASS ")(cn=" WILDCARD "))");

    syslog(LOG_DEBUG, MODPREFIX "searching for \"%s\"", query);
    rv = ldap_search_s(ldap, ctxt->base, LDAP_SCOPE_SUBTREE,
                       query, attrs, 0, &result);
    if( ( rv != LDAP_SUCCESS) || ( result == NULL ) ) {
      syslog(LOG_INFO, MODPREFIX "query failed for %s", query);
      free(query);
      return 1;
    }

    syslog(LOG_DEBUG, MODPREFIX "getting first entry for cn=\"/\"");

    e = ldap_first_entry(ldap, result);
  }

  if( e == NULL ) {
    syslog(LOG_INFO, MODPREFIX "got answer, but no first entry for %s", query);
    free(query);
    return 1;
  } else {
    syslog(LOG_DEBUG, MODPREFIX "examining first entry");
  }

  values = ldap_get_values(ldap, e, ATTRIBUTE);
  if( values == NULL ) {
    syslog(LOG_INFO, MODPREFIX "no " ATTRIBUTE " defined for %s", query);
    free(query);
    return 1;
  }

  /* Try each of the answers in sucession. */
  rv = 1;
  for( i = 0 ; ( values[i] != NULL ) && ( rv != 0 ) ; i++ ) {
    rv = ctxt->parser->parse_mount(root, name, name_len, values[0],
                 ctxt->parser->context);
  }

  /* Clean up. */
  ldap_value_free(values);
  ldap_msgfree(result);
  ldap_unbind(ldap);
  free(query);

  return rv;
}

/*
 * This destroys a context for queries to this module.  It releases the parser
 * structure (unloading the module) and frees the memory used by the context.
 */
int lookup_done(void *context)
{
  struct lookup_context *ctxt = (struct lookup_context *) context;
  int rv = close_parse(ctxt->parser);
  free(ctxt->server);
  free(ctxt->base);
  free(ctxt);
  return rv;
}
