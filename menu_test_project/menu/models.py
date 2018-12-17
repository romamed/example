from django.db import models

class Item(models.Model):
    name = models.CharField(max_length=100, primary_key=True)

class Tree(models.Model):
    menu_item = models.ForeignKey(Item, on_delete=models.CASCADE)
    left_num = models.IntegerField(primary_key=True)
    right_num = models.IntegerField()
    depth =  models.IntegerField()