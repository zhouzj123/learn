# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models

# Create your models here.
class BookInfo(models.Model):
    btitle = models.CharField(max_length=20)
    bpub_date = models.DateTimeField()
    def __str__(self):
        return  "%d" % self.pk

class HeroInfo(models.Model):
    hname = models.CharField(max_length=20)
    hgender = models.BooleanField()
    hcontent = models.CharField(max_length=100)
    hbook = models.ForeignKey(BookInfo)
    def __str__(self):
        return "%d" % self.pk

    def gender(self):
        if self.hgender:
            return '男'
        else:
            return '女'
    gender.short_description = '性别'