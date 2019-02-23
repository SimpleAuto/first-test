#!/usr/bin/env python
# -*- coding=utf-8 -*-
########################################################################
#    File Name: turtle.py
# 
#       Author: Thomas Beijing,Inc.
#         Mail: 982737593@qq.com
# Created Time: 2019年02月23日 星期六 23时43分47秒
#  Description: ...
#                
########################################################################
import turtle

turtle.color('blue')
turtle.penup()
turtle.goto(-110, -25)
turtle.pendown()
turtle.circle(45)

turtle.color('black')
turtle.penup()
turtle.goto(0, -25)
turtle.pendown()
turtle.circle(45)

turtle.color('red')
turtle.penup()
turtle.goto(110, -25)
turtle.pendown()
turtle.circle(45)

turtle.color('orange')
turtle.penup()
turtle.goto(-55, -75)
turtle.pendown()
turtle.circle(45)

turtle.color('green')
turtle.penup()
turtle.goto(55, -75)
turtle.pendown()
turtle.circle(45)

turtle.done()
