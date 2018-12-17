from django.contrib import admin

from .models import Item
from .models import Tree

admin.site.register(Item)
admin.site.register(Tree)