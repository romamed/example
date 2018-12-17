# Generated by Django 2.1 on 2018-12-17 12:06

from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    initial = True

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='Item',
            fields=[
                ('name', models.CharField(max_length=100, primary_key=True, serialize=False)),
            ],
        ),
        migrations.CreateModel(
            name='Tree',
            fields=[
                ('left_num', models.IntegerField(primary_key=True, serialize=False)),
                ('right_num', models.IntegerField()),
                ('depth', models.IntegerField()),
                ('menu_item', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='menu.Item')),
            ],
        ),
    ]