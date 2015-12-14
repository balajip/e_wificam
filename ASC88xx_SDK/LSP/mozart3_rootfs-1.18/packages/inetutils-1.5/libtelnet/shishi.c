/* Copyright (C) 2002, 2003, 2007 Simon Josefsson
   Copyright (C) 2003 Free Software Foundation, Inc.

This file is part of Shishi / GNU Inetutils.

Shishi is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

Shishi is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Shishi; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA. */

/* Written by Simon Josefsson and Nicolas Pouvesle, based on
   kerberos5.c from GNU InetUtils. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef SHISHI
#include <stdlib.h>
#include <stdio.h>
#include <arpa/telnet.h>
#include <shishi.h>
#include <assert.h>

#include <netdb.h>
#include <ctype.h>
#include <syslog.h>
#ifdef  HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#include "auth.h"
#include "misc.h"

#ifdef  ENCRYPTION
#include "encrypt.h"
#endif

Shishi_key  *enckey = NULL;

static unsigned char str_data[2048] = { IAC, SB, TELOPT_AUTHENTICATION, 0,
  AUTHTYPE_KERBEROS_V5,
};

#define KRB_AUTH             0	/* Authentication data follows */
#define KRB_REJECT           1	/* Rejected (reason might follow) */
#define KRB_ACCEPT           2	/* Accepted */
#define KRB_RESPONSE         3	/* Response for mutual auth. */

Shishi *shishi_handle = 0;
Shishi_ap *auth_handle;

#define DEBUG(c) if (auth_debug_mode) printf c

static int
Data (TN_Authenticator * ap, int type, unsigned char *d, int c)
{
  unsigned char *p = str_data + 4;
  unsigned char *cd = (unsigned char *) d;

  if (c == -1)
    c = strlen (cd);

  if (auth_debug_mode)
    {
      printf ("%s:%d: [%d] (%d)",
	      str_data[3] == TELQUAL_IS ? ">>>IS" : ">>>REPLY",
	      str_data[3], type, c);
      printd (d, c);
      printf ("\r\n");
    }

  *p++ = ap->type;
  *p++ = ap->way;
  *p++ = type;

  while (c-- > 0)
    {
      if ((*p++ = *cd++) == IAC)
	*p++ = IAC;
    }
  *p++ = IAC;
  *p++ = SE;
  if (str_data[3] == TELQUAL_IS)
    printsub ('>', &str_data[2], p - &str_data[2]);
  return (net_write (str_data, p - str_data));
}

Shishi * shishi_telnet = NULL;

/* FIXME: Reverse return code! */
int
krb5shishi_init (TN_Authenticator * ap, int server)
{
  if (server)
    {
      str_data[3] = TELQUAL_REPLY;
      if (!shishi_handle && shishi_init_server (&shishi_handle) != SHISHI_OK)
	return 0;
    }
  else
    {
      str_data[3] = TELQUAL_IS;
      if (!shishi_handle && shishi_init (&shishi_handle) != SHISHI_OK)
	return 0;
    }

  return 1;
}

void
krb5shishi_cleanup (TN_Authenticator * ap)
{
  if (shishi_handle == 0)
    return;

  shishi_done (shishi_handle);
  shishi_handle = 0;
}

int
krb5shishi_send (TN_Authenticator * ap)
{
  int ap_opts;
  char type_check[2];
  Shishi_tkt *tkt;
  Shishi_tkts_hint hint;
  Shishi_key *subkey;
  int rc;
  char *tmp;
  char *apreq;
  size_t apreq_len;

  tmp = malloc (strlen ("host/") + strlen (RemoteHostName) + 1);
  sprintf (tmp, "host/%s", RemoteHostName);
  memset (&hint, 0, sizeof (hint));
  hint.server = tmp;
  tkt = shishi_tkts_get (shishi_tkts_default (shishi_handle), &hint);
  free (tmp);
  if (!tkt)
    {
      DEBUG (("telnet: Kerberos V5: no shishi ticket for server\r\n"));
      return 0;
    }

  if (auth_debug_mode)
    shishi_tkt_pretty_print (tkt, stdout);

  if (!UserNameRequested)
    {
      DEBUG (("telnet: Kerberos V5: no user name supplied\r\n"));
      return 0;
    }

  if ((ap->way & AUTH_HOW_MASK) == AUTH_HOW_MUTUAL)
    ap_opts = SHISHI_APOPTIONS_MUTUAL_REQUIRED;
  else
    ap_opts = 0;

#ifdef ENCRYPTION
  ap_opts |= SHISHI_APOPTIONS_USE_SESSION_KEY;
#endif	/* ENCRYPTION */

  type_check[0] = ap->type;
  type_check[1] = ap->way;

#ifndef DONT_ALWAYS_USE_DES
  /* Even if we are not using a DES key, we can still try a DES
     session-key.  Then we can support DES_?FB64 encryption with 3DES
     or AES keys against non-RFC 2952 implementations.  Of course, it
     would be better to follow RFC 2952, but enc_des.c does not
     support this (see comment in that file). */
  if (shishi_tkt_keytype_fast (tkt) != SHISHI_DES_CBC_CRC &&
      shishi_tkt_keytype_fast (tkt) != SHISHI_DES_CBC_MD4 &&
      shishi_tkt_keytype_fast (tkt) != SHISHI_DES_CBC_MD5)
    rc = shishi_ap_etype_tktoptionsdata (shishi_handle, &auth_handle,
					 SHISHI_DES_CBC_MD5,
					 tkt, ap_opts,
					 (char *) &type_check, 2);
  else
#endif
    rc = shishi_ap_tktoptionsdata (shishi_handle, &auth_handle,
				   tkt, ap_opts, (char *) &type_check, 2);
  if (rc != SHISHI_OK)
    {
      DEBUG (("telnet: Kerberos V5: Could not make AP-REQ (%s)\r\n",
	      shishi_strerror (rc)));
      return 0;
    }

#ifdef ENCRYPTION
  if (enckey)
    {
      shishi_key_done (enckey);
      enckey = NULL;
    }

  rc = shishi_authenticator_get_subkey
    (shishi_handle, shishi_ap_authenticator (auth_handle), &enckey);
  if (rc != SHISHI_OK)
    {
      DEBUG (("telnet: Kerberos V5: could not get encryption key (%s)\r\n",
	      shishi_strerror (rc)));
      return 0;
    }
#endif

  rc = shishi_ap_req_der (auth_handle, &apreq, &apreq_len);
  if (rc != SHISHI_OK)
    {
      DEBUG (("telnet: Kerberos V5: could not DER encode (%s)\r\n",
	      shishi_strerror (rc)));
      return 0;
    }

  if (auth_debug_mode)
    {
      shishi_authenticator_print (shishi_handle, stdout,
				  shishi_ap_authenticator (auth_handle));
      shishi_apreq_print (shishi_handle, stdout,
			  shishi_ap_req (auth_handle));
    }

  if (!auth_sendname (UserNameRequested, strlen (UserNameRequested)))
    {
      DEBUG (("telnet: Not enough room for user name\r\n"));
      return 0;
    }

  if (!Data (ap, KRB_AUTH, apreq, apreq_len))
    {
      DEBUG (("telnet: Not enough room for authentication data\r\n"));
      return 0;
    }

  DEBUG (("telnet: Sent Kerberos V5 credentials to server\r\n"));

  return 1;
}

#ifdef ENCRYPTION
void
shishi_init_key (Session_Key * skey, int type)
{
  int32_t etype = shishi_key_type (enckey);

  if (etype == SHISHI_DES_CBC_CRC ||
      etype == SHISHI_DES_CBC_MD4 ||
      etype == SHISHI_DES_CBC_MD5)
    skey->type = SK_DES;
  else
    skey->type = SK_OTHER;
  skey->length = shishi_key_length (enckey);
  skey->data = shishi_key_value (enckey);

  encrypt_session_key (skey, type);
}
#endif

void
krb5shishi_reply (TN_Authenticator * ap, unsigned char *data, int cnt)
{
  static int mutual_complete = 0;
#ifdef ENCRYPTION
  Session_Key skey;
#endif

  if (cnt-- < 1)
    return;

  switch (*data++)
    {
    case KRB_REJECT:
      if (cnt > 0)
	printf ("[ Kerberos V5 refuses authentication because %.*s ]\r\n",
		cnt, data);
      else
	printf ("[ Kerberos V5 refuses authentication ]\r\n");
      auth_send_retry ();
      return;

    case KRB_ACCEPT:
      if (!mutual_complete)
	{
	  if ((ap->way & AUTH_HOW_MASK) == AUTH_HOW_MUTUAL)
	    {
	      printf ("[ Kerberos V5 accepted you, "
		      "but didn't provide mutual authentication! ]\r\n");
	      auth_send_retry ();
	      break;
	    }
#ifdef ENCRYPTION	  
	  shishi_init_key (&skey, 0);
#endif
	}

      if (cnt)
	printf ("[ Kerberos V5 accepts you as ``%.*s''%s ]\r\n", cnt, data,
		mutual_complete ?
		" (server authenticated)" : " (server NOT authenticated)");
      else
	printf ("[ Kerberos V5 accepts you ]\r\n");
      auth_finished (ap, AUTH_USER);
      break;

    case KRB_RESPONSE:
      if ((ap->way & AUTH_HOW_MASK) == AUTH_HOW_MUTUAL)
	{
	  if (shishi_ap_rep_verify_der (auth_handle, data, cnt) != SHISHI_OK)
	    {
	      printf ("[ Mutual authentication failed ]\r\n");
	      auth_send_retry ();
	      break;
	    }

	  if (auth_debug_mode)
	    {
	      shishi_aprep_print (shishi_handle, stdout,
				  shishi_ap_rep (auth_handle));
	      shishi_encapreppart_print (shishi_handle, stdout,
					 shishi_ap_encapreppart
					 (auth_handle));
	    }

#ifdef ENCRYPTION
	  shishi_init_key (&skey, 0);
#endif
	  mutual_complete = 1;
	}
      break;

    default:
      DEBUG (("Unknown Kerberos option %d\r\n", data[-1]));
    }  
}

int
krb5shishi_status (TN_Authenticator * ap, char *name, int level)
{
  char *cname;
  int cnamelen;
  int rc;
  int status;
  
  if (level < AUTH_USER)
    return level;

  rc = shishi_encticketpart_client
                   (shishi_handle,
		    shishi_tkt_encticketpart (shishi_ap_tkt (auth_handle)),
		    &cname, &cnamelen);

  if (UserNameRequested
      && rc == SHISHI_OK
      && cnamelen == strlen (UserNameRequested)
      && memcmp (UserNameRequested, cname, cnamelen) == 0)
    {
      /* FIXME: Check buffer length */
      strcpy (name, UserNameRequested);
      status = AUTH_VALID;
    }
  else
    status = AUTH_USER;
  free (cname);
  return status;
}

int
krb5shishi_is_auth (TN_Authenticator * a, unsigned char *data, int cnt,
		    char *errbuf, int errbuflen)
{
  Shishi_key *key, *key2;
  int rc;
  char *cnamerealm;
  int cnamerealmlen;
#ifdef ENCRYPTION
  Session_Key skey;
#endif

  rc = shishi_ap (shishi_handle, &auth_handle);
  if (rc != SHISHI_OK)
    {
      snprintf (errbuf, errbuflen,
		"Cannot allocate authentication structures: %s",
		shishi_strerror (rc));
      return 1;
    }

  rc = shishi_ap_req_der_set (auth_handle, data, cnt);
  if (rc != SHISHI_OK)
    {
      snprintf (errbuf, errbuflen,
		"Cannot parse authentication information: %s",
		shishi_strerror (rc));
      return 1;
    }

  key = shishi_hostkeys_for_localservice (shishi_handle, "host");
  if (key == NULL)
    {
      snprintf (errbuf, errbuflen, "Could not find key:\n%s\n",
		shishi_error (shishi_handle));
      return 1;
    }

  rc = shishi_ap_req_process (auth_handle, key);
  if (rc != SHISHI_OK)
    {
      snprintf (errbuf, errbuflen, "Could not process AP-REQ: %s\n",
		shishi_strerror (rc));
      return 1;
    }

  if (shishi_apreq_mutual_required_p (shishi_handle,
				      shishi_ap_req (auth_handle)))
    {
      char *der;
      size_t derlen;

      rc = shishi_ap_rep_der (auth_handle, &der, &derlen);
      if (rc != SHISHI_OK)
	{
	  snprintf (errbuf, errbuflen, "Error DER encoding aprep: %s\n",
		    shishi_strerror (rc));
	  return 1;
	}

      Data (a, KRB_RESPONSE, der, derlen);
      free (der);
    }

  rc = shishi_encticketpart_crealm (shishi_handle,
		    shishi_tkt_encticketpart (shishi_ap_tkt (auth_handle)),
					    &cnamerealm, &cnamerealmlen);
  if (rc != SHISHI_OK)
    {
      snprintf (errbuf, errbuflen, "Error getting authenticator name: %s\n",
		shishi_strerror (rc));
      return 1;
    }
  Data (a, KRB_ACCEPT, cnamerealm, cnamerealm ? -1 : 0);
  DEBUG (("telnetd: Kerberos5 identifies him as ``%s''\r\n",
	  cnamerealm ? cnamerealm : ""));
  free (cnamerealm);
  auth_finished (a, AUTH_USER);

#ifdef ENCRYPTION
  if (enckey)
    {
      shishi_key_done (enckey);
      enckey = NULL;
    }

  rc = shishi_authenticator_get_subkey (shishi_handle,
					shishi_ap_authenticator (auth_handle),
					&enckey);
  if (rc != SHISHI_OK)
    {
      Shishi_tkt * tkt;

      tkt = shishi_ap_tkt (auth_handle);
      if (tkt)
	{
	  rc = shishi_encticketpart_get_key (shishi_handle,
					     shishi_tkt_encticketpart (tkt),
					     &enckey);
	  if (rc != SHISHI_OK)
	    enckey = NULL;

	  shishi_tkt_done (tkt);
	}
    }

  if (enckey == NULL)
    {
      snprintf (errbuf, errbuflen,
		"telnet: Kerberos V5: could not get encryption key (%s)\r\n",
		shishi_strerror (rc));
      return 1;
    }

  shishi_init_key (&skey, 1);
#endif

  return 0;
}

void
krb5shishi_is (TN_Authenticator * ap, unsigned char *data, int cnt)
{
  int r = 0;
  char errbuf[512];

  puts ("krb5shishi_is");

  if (cnt-- < 1)
    return;
  errbuf[0] = 0;
  switch (*data++)
    {
    case KRB_AUTH:
      r = krb5shishi_is_auth (ap, data, cnt, errbuf, sizeof errbuf);
      break;

    default:
      DEBUG (("Unknown Kerberos option %d\r\n", data[-1]));
      Data (ap, KRB_REJECT, 0, 0);
      break;
    }

  if (r)
    {
      if (!errbuf[0])
	snprintf (errbuf, sizeof errbuf, "kerberos_is: error");
      Data (ap, KRB_REJECT, errbuf, -1);
      DEBUG (("%s\r\n", errbuf));
      syslog (LOG_ERR, "%s", errbuf);
    }
}

static char *
req_type_str (int type)
{
  switch (type)
    {
    case KRB_REJECT:
      return "REJECT";

    case KRB_ACCEPT:
      return "ACCEPT";

    case KRB_AUTH:
      return "AUTH";

    case KRB_RESPONSE:
      return "RESPONSE";

    }
  return NULL;
}

#define ADDC(p,l,c) if ((l) > 0) {*(p)++ = (c); --(l);}

void
krb5shishi_printsub (unsigned char *data, int cnt,
		     unsigned char *buf, int buflen)
{
  char *p;
  int i;

  puts ("krb5shishi_printsub");

  buf[buflen - 1] = '\0';	/* make sure its NULL terminated */
  buflen -= 1;

  p = req_type_str (data[3]);
  if (!p)
    {
      int l = snprintf (buf, buflen, " %d (unknown)", data[3]);
      buf += l;
      buflen -= l;
    }
  else
    {
      while (buflen > 0 && (*buf++ = *p++) != 0)
	buflen--;
    }

  switch (data[3])
    {
    case KRB_REJECT:		/* Rejected (reason might follow) */
    case KRB_ACCEPT:		/* Accepted (username might follow) */
      if (cnt <= 4)
	break;
      ADDC (buf, buflen, '"');
      for (i = 4; i < cnt; i++)
	ADDC (buf, buflen, data[i]);
      ADDC (buf, buflen, '"');
      ADDC (buf, buflen, '\0');
      break;

    case KRB_AUTH:
    case KRB_RESPONSE:
      for (i = 4; buflen > 0 && i < cnt; i++)
	{
	  int l = snprintf (buf, buflen, " %d", data[i]);
	  buf += l;
	  buflen -= l;
	}
    }
}

#endif /* SHISHI */
