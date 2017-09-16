#!/usr/bin/env bash
########################################################################
#    File Name: onekey_git.sh
# 
#       Author: Thomas Shanghai,Inc.
#         Mail: 982737593@qq.com
# Created Time: 2017年09月16日 星期六 19时50分09秒
#  Description: ...
# 
########################################################################

ls_date=`date +%Y%m%d`
echo $ls_date;
git add .
echo git add .
git commit -m \"$ls_date\" .
echo git commit -m \"$ls_date\" .
git push -u origin master
echo git push -u origin master
