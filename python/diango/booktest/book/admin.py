# -*- coding: utf-8 -*-
from __future__ import unicode_literals
# Register your models here.
from django.contrib import admin
from models import BookInfo, HeroInfo

class HeroInfoInline(admin.TabularInline):
    model = HeroInfo
    extra = 3

class BookInfoAdmin(admin.ModelAdmin):
    list_display = ['pk', 'btitle', 'bpub_date']
    list_filter = ['btitle']
    search_fields = ['btitle']
    list_per_page = 10

    fieldsets = [
        ('basic', {'fields': ['btitle']}),
        ('more', {'fields': ['bpub_date']}),
    ]
    inlines = [HeroInfoInline]

class HeroInfoAdmin(admin.ModelAdmin):
    list_display = ['id', 'hname', 'gender', 'hcontent']

admin.site.register(BookInfo, BookInfoAdmin)
admin.site.register(HeroInfo, HeroInfoAdmin)
