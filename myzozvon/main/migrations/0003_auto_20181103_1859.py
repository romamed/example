# Generated by Django 2.1.2 on 2018-11-03 18:59

from django.db import migrations


class Migration(migrations.Migration):

    dependencies = [
        ('main', '0002_auto_20181103_1853'),
    ]

    operations = [
        migrations.RenameField(
            model_name='image',
            old_name='scr',
            new_name='src',
        ),
    ]