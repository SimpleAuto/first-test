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

dir=~/grade/first-test
ls_date=`date +%Y%m%d`
echo $ls_date;
echo $dir;
git add $dir
echo "=========="
echo git add $dir [OK]
echo "=========="
git commit -m \"$ls_date\"  $dir
echo "=========="
echo git commit -m \"$ls_date\" $dir [OK]
echo "=========="
git push -u origin master
echo "=========="
echo git push -u origin master [OK]
echo "=========="
