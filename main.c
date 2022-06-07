#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct huffman *huffmanKok;


struct lz77
{
    char **aramaTamponlari;
    int aramaTokenSayisi;
    int *kullanildimi;

};

struct huffman
{
    char *karakterler;
    int frekans;

    struct huffman *sag;
    struct huffman *sol;
};

struct lz77Token
{
    int uzunluk;
    int uzaklik;
    char karakter;
};



struct lz77 lz77TokenAyirma(char*,struct lz77);

void lz77Sifreleme(char*,struct lz77,struct lz77Token*);

int uzunlukBulma(char*,int,int,int,struct lz77);

void lz77Yazdirma(struct lz77,struct lz77Token*,char*);

void huffmanAgaciOlusturma(struct huffman **huffmanDizisi,int boyut,char *huffmanKarakterleri);

void huffmanYazdirma(struct huffman *kok,char *karakterler,int boyut);

void deflateYazdirma(int *kullanildimi,struct huffman *kok,struct lz77Token *tokenler,char *sifrelenmemis);

void lz77VsDeflate();













int main()
{

    FILE *dosya=fopen("metin.txt","rb");

    if(dosya==NULL)
    {
        printf("dosyaniz acilamadi \n");

        return 0;
    }

    char *sifrelenmemis=(char*)malloc(sizeof(char)*1);

    int indis=1;

    char kontrol=fgetc(dosya);

    rewind(dosya);

    while(kontrol!=EOF)
    {
        kontrol=fgetc(dosya);

        if(kontrol==EOF)
        {
            break;
        }

        sifrelenmemis[indis-1]=kontrol;

        indis++;

        sifrelenmemis=realloc(sifrelenmemis,sizeof(char)*indis);
    }

    fclose(dosya);

    sifrelenmemis[indis-1]='\0';


    struct lz77 tampon;

    struct lz77Token *tokenler=(struct lz77Token*)malloc(sizeof(struct lz77Token)*strlen(sifrelenmemis));

    tampon=lz77TokenAyirma(sifrelenmemis,tampon);

    lz77Sifreleme(sifrelenmemis,tampon,tokenler);

    lz77Yazdirma(tampon,tokenler,sifrelenmemis);



    int huffmanDizisiBoyutu=1;

    struct huffman **huffmanDizisi=(struct huffman**)malloc(sizeof(struct huffman*)*huffmanDizisiBoyutu);

    huffmanDizisi[0]=(struct huffman*)malloc(sizeof(struct huffman)*1);

    huffmanDizisi[0]->frekans=0;

    huffmanDizisi[0]->sag=NULL;
    huffmanDizisi[0]->sol=NULL;

    huffmanDizisi[0]->karakterler=(char*)malloc(sizeof(char)*2);
    huffmanDizisi[0]->karakterler[0]=sifrelenmemis[0];
    huffmanDizisi[0]->karakterler[1]='\0';

    int i,j;

    for( i=0;i<strlen(sifrelenmemis);i++)
    {
        int mevcutmu=0;

        if(tampon.kullanildimi[i]==0)
        {
            for( j=0;j<huffmanDizisiBoyutu;j++)
            {
                if(sifrelenmemis[i]==huffmanDizisi[j]->karakterler[0])
                {
                    huffmanDizisi[j]->frekans++;

                    mevcutmu=1;

                    break;
                }
            }

            if(mevcutmu==0)
            {
                huffmanDizisi=(struct huffman**)realloc(huffmanDizisi,sizeof(struct huffman*)*++huffmanDizisiBoyutu);

                huffmanDizisi[huffmanDizisiBoyutu-1]=(struct huffman*)malloc(sizeof(struct huffman)*1);

                huffmanDizisi[huffmanDizisiBoyutu-1]->frekans=1;

                huffmanDizisi[huffmanDizisiBoyutu-1]->karakterler=(char*)malloc(sizeof(char)*2);
                huffmanDizisi[huffmanDizisiBoyutu-1]->karakterler[0]=sifrelenmemis[i];
                huffmanDizisi[huffmanDizisiBoyutu-1]->karakterler[1]='\0';

                huffmanDizisi[huffmanDizisiBoyutu-1]->sag=NULL;
                huffmanDizisi[huffmanDizisiBoyutu-1]->sol=NULL;
            }
        }
    }

    char *huffmanKarakterleri=(char*)malloc(sizeof(char)*huffmanDizisiBoyutu);

    huffmanAgaciOlusturma(huffmanDizisi,huffmanDizisiBoyutu,huffmanKarakterleri);

    huffmanYazdirma(huffmanKok,huffmanKarakterleri,huffmanDizisiBoyutu);

    deflateYazdirma(tampon.kullanildimi,huffmanKok,tokenler,sifrelenmemis);

    lz77VsDeflate();





   return 0;
}

struct lz77 lz77TokenAyirma(char *sifrelenmemis,struct lz77 tampon)
{
     tampon.aramaTokenSayisi=strlen(sifrelenmemis)/7;
    int aramaTokenSirasi=0;
    int aramaTokenIndisi=0;
    int sifrelenmemisIndis=0;



    tampon.aramaTamponlari=(char**)malloc(sizeof(char*)*tampon.aramaTokenSayisi);
    tampon.kullanildimi=(int*)malloc(sizeof(int)*strlen(sifrelenmemis));

    int i;

    for( i=0;i<strlen(sifrelenmemis);i++)
    {
        tampon.kullanildimi[i]=0;
    }





    for( i=0;i<tampon.aramaTokenSayisi;i++)
    {

        tampon.aramaTamponlari[i]=(char*)malloc(sizeof(char)*8);
    }





    while(aramaTokenSirasi!=tampon.aramaTokenSayisi)
    {



        tampon.aramaTamponlari[aramaTokenSirasi][aramaTokenIndisi]=sifrelenmemis[sifrelenmemisIndis];



        aramaTokenIndisi++;
        sifrelenmemisIndis++;

        if(aramaTokenIndisi%7==0)
        {
            tampon.aramaTamponlari[aramaTokenSirasi][aramaTokenIndisi]='\0';
            aramaTokenSirasi++;
            aramaTokenIndisi=0;

        }
    }

    return tampon;

}

void lz77Sifreleme(char *sifrelenmemis,struct lz77 tampon,struct lz77Token *tokenler)
{

    int sinir=0;

    int uzunluk=0;
    int uzaklik=0;


   int sifrelenmemisIndis=7;

   int aramaSirasi,aramaIndisi;

   int i;


    while(sifrelenmemisIndis<strlen(sifrelenmemis))
    {

        sinir=sifrelenmemisIndis/7;

        for( aramaSirasi=0;aramaSirasi<sinir;aramaSirasi++)
        {
            for( aramaIndisi=0;aramaIndisi<7;aramaIndisi++)
            {
                if(sifrelenmemis[sifrelenmemisIndis]==tampon.aramaTamponlari[aramaSirasi][aramaIndisi])
                {
                    uzunluk=uzunlukBulma(sifrelenmemis,sifrelenmemisIndis,aramaSirasi,aramaIndisi,tampon);

                    if(uzunluk>1)
                    {
                        int uygunmu=1;



                        for( i=0;i<uzunluk;i++)
                        {
                            if(tampon.kullanildimi[sifrelenmemisIndis+i]==1)
                            {
                                uygunmu=0;
                                break;
                            }
                        }

                        if(uygunmu==1)
                        {
                            uzaklik=sifrelenmemisIndis-((aramaSirasi*7)+aramaIndisi);

                            //printf("baslangic indisi = %d\t uzaklik = %d\t uzunluk=%d\n",sifrelenmemisIndis,uzaklik,uzunluk);

                            for( i=0;i<uzunluk;i++)
                            {
                                tampon.kullanildimi[sifrelenmemisIndis+i]=1;

                            }

                            tokenler[sifrelenmemisIndis].karakter=sifrelenmemis[sifrelenmemisIndis];
                            tokenler[sifrelenmemisIndis].uzaklik=uzaklik;
                            tokenler[sifrelenmemisIndis].uzunluk=uzunluk;





                        }
                    }

                    uzunluk=0;
                    uzaklik=0;
                }
            }
        }

        sifrelenmemisIndis++;

    }








}

int uzunlukBulma(char *sifrelenmemis,int sifrelenmemisIndis,int aramaSirasi,int aramaIndisi,struct lz77 tampon)
{
    int uzunluk=0;

    while(1)
    {
        if(sifrelenmemis[sifrelenmemisIndis]!=tampon.aramaTamponlari[aramaSirasi][aramaIndisi]||sifrelenmemisIndis==strlen(sifrelenmemis)||aramaIndisi==7)
        {
            break;
        }

        uzunluk++;
        sifrelenmemisIndis++;
        aramaIndisi++;
    }

    return uzunluk;
}

void lz77Yazdirma(struct lz77 tampon,struct lz77Token *tokenler,char *sifrelenmemis)
{
    FILE *dosya=fopen("lz77 ciktisi.txt","w");

    int i;

    if(dosya==NULL)
    {
        printf("Cikti dosyasi yazdirma basarisiz \n");
    }

    for( i=0;i<strlen(sifrelenmemis);i++)
    {
        if(tampon.kullanildimi[i]==0)
        {
            fprintf(dosya,"<0,0,%c>\n",sifrelenmemis[i]);
        }

        else
        {
            fprintf(dosya,"<%d,%d,%c>\n",tokenler[i].uzaklik,tokenler[i].uzunluk,tokenler[i].karakter);

            i+=tokenler[i].uzunluk-1;
        }
    }

    fclose(dosya);

}

void huffmanAgaciOlusturma(struct huffman **dizi,int boyut,char *huffmanKarakterleri)
{

    huffmanKok=(struct huffman*)malloc(sizeof(struct huffman)*1);

    int i,j;

    for( i=0;i<boyut;i++)
    {
        huffmanKarakterleri[i]=dizi[i]->karakterler[0];
    }

    while(1)
    {
        for(i=0;i<boyut;i++)
        {
            for(j=i+1;j<boyut;j++)
            {
                if(dizi[i]->frekans>dizi[j]->frekans)
                {
                    struct huffman *yertutma=(struct huffman*)malloc(sizeof(struct huffman)*1);

                    yertutma=dizi[i];
                    dizi[i]=dizi[j];
                    dizi[j]=yertutma;
                }
            }
        }

        if(boyut==2)
        {
            huffmanKok->sol=dizi[0];
            huffmanKok->sag=dizi[1];

            huffmanKok->frekans=huffmanKok->sag->frekans+huffmanKok->sol->frekans;

            huffmanKok->karakterler=(char*)malloc(sizeof(char)*(strlen(dizi[0]->karakterler)+strlen(dizi[1]->karakterler)+1));

        strcpy(huffmanKok->karakterler,dizi[0]->karakterler);
        strcpy(huffmanKok->karakterler+strlen(dizi[0]->karakterler),dizi[1]->karakterler);
        huffmanKok->karakterler[strlen(dizi[0]->karakterler)+strlen(dizi[1]->karakterler)]='\0';

            break;
        }


        struct huffman *yeni=(struct huffman*)malloc(sizeof(struct huffman)*1);

        yeni->sol=dizi[0];
        yeni->sag=dizi[1];

        yeni->frekans=dizi[0]->frekans+dizi[1]->frekans;

        yeni->karakterler=(char*)malloc(sizeof(char)*(strlen(dizi[0]->karakterler)+strlen(dizi[1]->karakterler)+1));

        strcpy(yeni->karakterler,dizi[0]->karakterler);
        strcpy(yeni->karakterler+strlen(dizi[0]->karakterler),dizi[1]->karakterler);
        yeni->karakterler[strlen(dizi[0]->karakterler)+strlen(dizi[1]->karakterler)]='\0';

        dizi[0]=yeni;

        boyut--;

        int i;

        for(i=1;i<boyut;i++)
        {
            dizi[i]=dizi[i+1];
        }


    }


}

void huffmanYazdirma(struct huffman *kok,char *karakterler,int boyut)
{
    FILE *dosya=fopen("huffman.txt","w");
    struct huffman *adim;

    if(dosya==NULL)
    {
        printf("Dosya acilamadi");
    }

    int i;

   for( i=0;i<boyut;i++)
   {
        adim=kok;

        fprintf(dosya,"%c\t",karakterler[i]);

        while(1)
        {

            if(strchr(adim->sag->karakterler,karakterler[i])!=0)
            {
                fprintf(dosya,"1");

                if(strlen(adim->sag->karakterler)==1)
                {
                    break;
                }

                adim=adim->sag;
            }

            else if(strchr(adim->sol->karakterler,karakterler[i])!=0)
            {
                fprintf(dosya,"0");

                if(strlen(adim->sol->karakterler)==1)
                {
                    break;
                }

                adim=adim->sol;
            }
        }

        fprintf(dosya,"\n");
   }

   fclose(dosya);
}

void deflateYazdirma(int *kullanildimi,struct huffman *kok,struct lz77Token *tokenler,char *sifrelenmemis)
{
    FILE *dosya=fopen("deflate ciktisi.txt","w");

    int sayac;

    for( sayac=0;sayac<strlen(sifrelenmemis);sayac++)
    {
        if(kullanildimi[sayac]==1)
        {
            fprintf(dosya,"<%d,%d,%c>\n",tokenler[sayac].uzaklik,tokenler[sayac].uzunluk,tokenler[sayac].karakter);

            sayac+=tokenler[sayac].uzunluk-1;
        }

        else if(kullanildimi[sayac]==0)
        {
            struct huffman *adim=kok;

            while(1)
            {
                if(strchr(adim->sag->karakterler,sifrelenmemis[sayac])!=0)
                {
                    fprintf(dosya,"1");

                    if(strlen(adim->sag->karakterler)==1)
                    {
                        break;
                    }

                    adim=adim->sag;
                }

                else if(strchr(adim->sol->karakterler,sifrelenmemis[sayac])!=0)
                {
                    fprintf(dosya,"0");

                    if(strlen(adim->sol->karakterler)==1)
                    {
                        break;
                    }

                    adim=adim->sol;
                }
            }

            fprintf(dosya,"\n");
        }
    }

    fclose(dosya);
}

void lz77VsDeflate()
{
    FILE *dosya=fopen("lz77 ciktisi.txt","rb");

    if(dosya==NULL)
    {
        printf("lz77 dosyasi acilamadi \n");
    }

    fseek(dosya,0,SEEK_END);

    int lz77Boyutu=ftell(dosya);

    dosya=fopen("deflate ciktisi.txt","rb");

    if(dosya==NULL)
    {
        printf("deflate dosyasi acilamadi \n");
    }

    fseek(dosya,0,SEEK_END);

    int deflateBoyutu=ftell(dosya);

    fclose(dosya);

    printf("\ndeflate boyutu = %d\t lz77 boyutu = %d\n",deflateBoyutu,lz77Boyutu);

    if(lz77Boyutu>deflateBoyutu)
    {
        printf("Deflate ile sikistirilan dosya daha az yer kapliyor,onu kullanin!\n");
    }

    else if(lz77Boyutu<deflateBoyutu)
    {
        printf("Lz77 ile sikistirilan dosya daha az yer kapliyor,onu kullanin!\n");
    }

    else if(lz77Boyutu==deflateBoyutu)
    {
        printf("Iki yontem de ayný sonucu verdi,istediginizi kullanabilirsiniz \n");
    }
}








