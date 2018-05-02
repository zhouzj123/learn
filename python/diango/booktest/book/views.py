# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.shortcuts import render
from django.http import *
from .models import *
# Create your views here.
from models import BookInfo

def index(request):
    bookList = BookInfo.objects.all()
    context = {"booklist":bookList}

    return render(request, 'booktest/index.html',context)

def detail(request, id):
    book = BookInfo.objects.get(pk=id)
    context = {'book':book}
    return render(request,'booktest/detail.html',context)
