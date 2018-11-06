# Generated by Django 2.1.2 on 2018-11-04 07:48

from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    dependencies = [
        ('main', '0003_auto_20181103_1859'),
    ]

    operations = [
        migrations.AlterField(
            model_name='artist',
            name='birthday',
            field=models.DateField(blank=True, null=True),
        ),
        migrations.AlterField(
            model_name='artistactiveperiod',
            name='year_of_end',
            field=models.IntegerField(blank=True, null=True),
        ),
        migrations.AlterField(
            model_name='band',
            name='year_of_disband',
            field=models.IntegerField(blank=True, null=True),
        ),
        migrations.AlterField(
            model_name='image',
            name='album',
            field=models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, to='main.Album'),
        ),
        migrations.AlterField(
            model_name='image',
            name='band',
            field=models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, to='main.Band'),
        ),
    ]
