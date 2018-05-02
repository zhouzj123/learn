# -*- coding: utf-8 -*-
from __future__ import unicode_literals
from django.shortcuts import render
from django.http import *
from .models import *
# Create your views here.
from models import BookInfo, AreaInfo
from django.db.models import Max,F,Q
def index(request):
    list = BookInfo.books1.filter(Q(pk__lt=8)|Q(btitle__contains='1'))
    context = {'list1':list }
    return render(request, 'index.html',context)

def detail(request, id):
    book = BookInfo.objects.get(pk=id)
    context = {'book':book}
    return render(request,'detail.html',context)

def area(request):
    area = AreaInfo.objects.get(pk=130100)
    return request(request, 'area.html', {'area':area})