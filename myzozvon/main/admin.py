from django.contrib import admin

from .models import Country
from .models import Instrument
from .models import Artist
from .models import Band
from .models import ArtistActivePeriod
from .models import Genre
from .models import GenreOfBand
from .models import Album
from .models import Track
from .models import Image

admin.site.register(Country)
admin.site.register(Instrument)
admin.site.register(Artist)
admin.site.register(Band)
admin.site.register(ArtistActivePeriod)
admin.site.register(Genre)
admin.site.register(GenreOfBand)
admin.site.register(Album)
admin.site.register(Track)
admin.site.register(Image)