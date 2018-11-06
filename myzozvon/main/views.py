from django.shortcuts import render, redirect
from django.conf import settings
import requests
from django.db import connection
from .models import Band
from .models import Image
from .models import ArtistActivePeriod
from .models import GenreOfBand
from .models import Album
from .models import Track

def main(request):
    if not request.session.get('mysite', False):
        return redirect('/login')
    
    list = []
    l = []
    for img in Image.objects.raw('SELECT * FROM main_image'):
        if img.band_id:
            band = Band.objects.raw(f'SELECT * FROM main_band WHERE main_band.id = {img.band_id}')
            l.append(band[0].name)
            l.append(img.src)
            list.append(l)
            l = []

    context = {'list': list}
    
    return render(request, 'main/main.html', context)

def band(request, band):
    if not request.session.get('mysite', False):
        return redirect('/login')
    
    with connection.cursor() as cursor:
        cursor.execute("SELECT main_artist.first_name, main_artist.last_name, main_artistactiveperiod.instrument_id FROM main_artistactiveperiod, main_artist, main_band WHERE main_band.name = %s AND main_band.id = main_artistactiveperiod.band_id AND main_artistactiveperiod.artist_id = main_artist.id", [band])
        artists = cursor.fetchall()
        cursor.execute("SELECT main_genreofband.genre_id FROM main_genreofband, main_band WHERE main_band.name = %s AND main_band.id = main_genreofband.band_id", [band])
        genre = cursor.fetchall()
        cursor.execute("SELECT main_album.name FROM main_album, main_band WHERE main_band.name = %s AND main_band.id = main_album.band_id", [band])
        albums = cursor.fetchall()
    
    context = {'band': band,
               'artists': artists,
               'genre': genre,
               'albums': albums
               }
    
    return render(request, 'main/band.html', context)

def album(request, band, album):
    if not request.session.get('mysite', False):
        return redirect('/login')
    
    with connection.cursor() as cursor:
        cursor.execute("SELECT main_image.src FROM main_image, main_album WHERE main_album.name = %s AND main_album.id = main_image.album_id", [album])
        image = cursor.fetchall()
        cursor.execute("SELECT main_track.name, main_track.src FROM main_track, main_album WHERE main_album.name = %s AND main_album.id = main_track.album_id", [album])
        tracks = cursor.fetchall()
    
    return render(request, 'main/album.html', {'image': image, 'album': album, 'tracks': tracks})

def logout(request):
    if not request.session.get('mysite', False):
        return redirect('/login')
    
    del request.session['mysite']
    request.session.modified = True

    return redirect('login')
