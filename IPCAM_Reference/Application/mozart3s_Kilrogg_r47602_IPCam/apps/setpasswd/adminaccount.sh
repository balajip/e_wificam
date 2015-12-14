#!/bin/sh

#(0)  adminaccount.sh  -c create -u jeff -p 609602 -g vaviewer
#     adminaccount.sh  -c create -u killer -p 609602 -g vaadmin
#     adminaccount.sh  -c delete -u jeff
#     adminaccount.sh  -c export -o /tmp/account.txt
#     The format of each line in "/tmp/account.txt" is :
#     [group1]:	
#     [username1]
#     [username2]
#     ....
#     [group2]:
#     [username1]
#     [username2]
#     ....
#(1) There are two group (role) in the system (/etc/group):
#	vaadmin: (a) its gid is 102, (b) the user  belonging to the group can configure the IPCamera.
#	vaviewer: (b) its gid is 103, (b) the user  belonging to the group only can view the display.
#(2) Add the folloing web server configurations for managing the privilege of usage of IPCamera:
#	
#	PasswdFile /etc/passwd
#	GroupFile /etc/group
#  	Realm vaadmin_realm vaadmin
#	ProtRealm vaadmin_realm password
#	ProtPath vaadmin_realm /usr/share/www/html/setup
#	ProtSpace vaadmin_realm IP-Camera-vaadmin
#	Realm vaviewer_realm vaviewer
#	ProtRealm vaviewer_realm password
#	ProtPath vaviewer_realm /usr/share/www/html
#	ProtSpace vaviewer_realm IP-Camera-vaviewer
#
#(3) Comment all the statement about "root_realm".
#(4) After modifying the web server config, you must reload the web server for testing the managing function:
#    /etc/init.d/http  reload
#(5) Once you  use adminaccount.sh to manage the accounts, you must relad  the web server:
#   /etc/init.d/http  reload



command=""
username=""
passwd=""
group=""
outfile=""
gid=""
exist=""
group_entry=""
group_path="/etc/group"
passwd_path="/etc/passwd"
setpasswd_path="/usr/sbin/setpasswd"
args=`getopt o:g:p:u:c: $*`
if test $# = 0
then
      echo "Usage:"
      echo "adminaccount.sh  [-c create|delete|export ]"
      echo "adminaccount.sh  [-c create][-u username][-p password][-g group]"
      echo "adminaccount.sh  [-c delete][-u username]"
      echo "adminaccount.sh  [-c export][-o export_file_path]"
      echo "adminaccount.sh  [-c check][-u username]"
      echo "Options:"
      echo "-u username		username"
      echo "-p password		new password"
      echo "-g group		group name"
      echo "-o export_file_path	export account file path"
      exit 1
fi

set -- $args
for i
do
  case "$i" in
        -c) shift;command=$1;shift;;
        -u) shift;username=$1;shift;;
        -p) shift;passwd=$1;shift;;
	-g) shift;group=$1;shift;;
	-o) shift;outfile=$1;shift;;
	
  esac
done

if [ "$command" ];then
  #echo "command:"$command
  if [ $command = "create" ];then
      #echo "username:"$username
      #echo "passwd:"$passwd
      #echo "group:"$group
      if cat $passwd_path | grep -n $username   > /dev/null  2>&1
      then
	echo "\"$username\" has existed in \"$passwd_path\".Please delete it and then create it."
	exit 1
      fi
    
      if cat $group_path | grep -n $group   > /dev/null  2>&1
      then
	gid=`cat $group_path | grep -r $group":"  |  awk -F':' '{ print $3}'`
	passwd_str="$username::$gid:$gid:$group:/:/bin/sh"
	echo  $passwd_str >>  $passwd_path
	$setpasswd_path -u $username -p $passwd -f $passwd_path
#	group_str=`cat $group_path | grep -r "$group:"`
#	sed "/$group:/d" $group_path  > /tmp/group.tmp
#	mv /tmp/group.tmp $group_path
#	check_notfirst_user=`echo $group_str | awk -F':' '{ print $4}'`
#	if [ "$check_notfirst_user" ];then
#	  echo $group_str","$username >>  $group_path
#	else
#	  echo $group_str""$username >>  $group_path
#	fi
      fi
  elif [  $command = "delete" ];then
     # echo "username:"$username
      if cat $passwd_path | grep -n $username   > /dev/null  2>&1
      then
	sed "/$username/d" $passwd_path  > /tmp/passwd.tmp
	mv /tmp/passwd.tmp $passwd_path
#	if cat $group_path | grep -r ",$username"  > /dev/null  2>&1
#	then
#	  sed "s/,$username//g" $group_path > /tmp/group.tmp 
#	  mv /tmp/group.tmp $group_path
#	elif  cat $group_path | grep -r ":$username"  > /dev/null  2>&1
#	then 
#	   sed "s/$username//g" $group_path > /tmp/group.tmp 
#	  mv /tmp/group.tmp $group_path
#	fi
      else
	echo "\"$username\" does not exist in \"$passwd_path\""
	exit 1
      fi
       
  elif [  $command = "export" ];then
      #echo "outfile:"$outfile
      if [ -e $outfile ];then
	  rm $outfile
      fi
      touch $outfile
      #echo "begin:" >> $outfile
      cat $group_path | \
      while read entry; do
	group_name=`echo $entry |  awk -F':' '{ print $1}'`
	gid=`echo $entry |  awk -F':' '{ print $3}'`
	group_users=""
	#echo $group_name":" >> $outfile 
	cat $passwd_path | \
	while read item; do
	    u_gid=`echo $item |  awk -F':' '{ print $4}'`
	    if [ "$u_gid" = "$gid" ];then
		user=`echo $item |  awk -F':' '{ print $1}'`
		echo  $user":"$group_name >> $outfile 
	    fi
	done
      done 
      #echo "end:" >> $outfile

  elif [ $command = "check" ];then
     exist_check_file=`/bin/mktemp -t check.XXXXXX`
     echo "n" >  $exist_check_file
     cat $passwd_path | \
      while read entry; do
	user=`echo $entry |  awk -F':' '{ print $1}'`
	if [ "$user" = "$username" ];then
	   echo "y" >  $exist_check_file
	   break
	fi 
      done 

      exist=`cat $exist_check_file`
      if [ "$exist" = "y" ];then
	  echo "y"
      else
	 echo "n"
      fi
      rm $exist_check_file
  fi 
fi 

