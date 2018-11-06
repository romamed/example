from django.db import models

class Country(models.Model):
    name = models.CharField(max_length=100, primary_key=True)

class Instrument(models.Model):
    name = models.CharField(max_length=100, primary_key=True)

class Artist(models.Model):
    first_name = models.CharField(max_length=100)
    last_name = models.CharField(max_length=100)
    country = models.ForeignKey(Country, on_delete=models.CASCADE)
    birthday = models.DateField(null=True, blank=True)

class Band(models.Model):
    name = models.CharField(max_length=100)
    year_of_form = models.IntegerField()
    year_of_disband = models.IntegerField(null=True, blank=True)

class ArtistActivePeriod(models.Model):
    artist = models.ForeignKey(Artist, on_delete=models.CASCADE)
    instrument = models.ForeignKey(Instrument, on_delete=models.CASCADE)
    band = models.ForeignKey(Band, on_delete=models.CASCADE)
    year_of_start = models.IntegerField()
    year_of_end = models.IntegerField(null=True, blank=True)

class Genre(models.Model):
    name = models.CharField(max_length=100, primary_key=True)

class GenreOfBand(models.Model):
    genre = models.ForeignKey(Genre, on_delete=models.CASCADE)
    band = models.ForeignKey(Band, on_delete=models.CASCADE)

class Album(models.Model):
    band = models.ForeignKey(Band, on_delete=models.CASCADE)
    name = models.CharField(max_length=100)
    year = models.IntegerField()

class Track(models.Model):
    album = models.ForeignKey(Album, on_delete=models.CASCADE)
    name = models.CharField(max_length=100)
    src = models.CharField(max_length=100)

class Image(models.Model):
    src = models.CharField(max_length=100, primary_key=True)
    band = models.ForeignKey(Band, on_delete=models.CASCADE, null=True, blank=True)
    album = models.ForeignKey(Album, on_delete=models.CASCADE, null=True, blank=True)