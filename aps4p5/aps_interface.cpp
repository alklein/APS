#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "likelihoodinterface.h"

main(int iargc, char *argv[]){
   
   int i,j,k,l,ii,dim,nstart=2000,nend=10000,nc=200,llswit=0;
   int resumeswitch=0;
   char paramname[letters],keyword[letters],word[letters];
   char resumename[letters];
   double nn,mm,*min,*max,exc;
   
   double **guess,**buff;
   int nguess;
   
   nguess=0;
   
   likelihood *likeness;
   
   FILE *input;
   
   for(j=0;argv[1][j]!=0;j++){
     paramname[j]=argv[1][j];
   }
   paramname[j]=0;
   
   input=fopen(paramname,"r");
   while(fscanf(input,"%s",keyword)==1){
     if(compare_char(keyword,"#dim")==1){
       
       fscanf(input,"%d",&dim);
       
       min=new double[dim];
       max=new double[dim];
       
     }
     else{
       if(compare_char(keyword,"#param")==1){
         fscanf(input,"%d %s %le %le",&ii,word,&nn,&mm);
	 
	 if(nn>mm){
	   max[ii]=nn;
	   min[ii]=mm;
	 }
	 else{
	   max[ii]=mm;
	   min[ii]=nn;
	 }
	 
	 //printf("set lims %d %e %e\n",ii,min[ii],max[ii]);
	 
       }
     }
   }
   fclose(input);
   likeness=new likelihood(dim,min,max);
   
   likeness->pnames=new char*[dim];
   for(i=0;i<dim;i++)likeness->pnames[i]=new char[letters];
   for(i=0;i<dim;i++){
     sprintf(likeness->pnames[i],"param%d",i);
   }
   
   input=fopen(paramname,"r");
   while(fscanf(input,"%s",keyword)==1){
    if(compare_char(keyword,"#param")==1){
      fscanf(input,"%d %s %le %le",&ii,word,&nn,&mm);
      
      for(i=0;word[i]!=0;i++){
        likeness->pnames[ii][i]=word[i];
      }
      likeness->pnames[ii][i]=0;
      
      //printf("named %d %s\n",ii,likeness->pnames[ii]);
      
    }
    else if(compare_char(keyword,"#Ns")==1){
      fscanf(input,"%d",&nstart);
      likeness->npts=nstart;
      
      //printf("set start to %d\n",likeness->npts);
      
    }
    else if(compare_char(keyword,"#exception")==1){
      fscanf(input,"%le",&exc);
      likeness->chiexcept=exc;
    }
    else if(compare_char(keyword,"#end")==1){
      fscanf(input,"%d",&nend);
      
      //printf("set end %d\n",nend);
      
    }
    else if(compare_char(keyword,"#Nc")==1){
      fscanf(input,"%d",&nc);
      
      //printf("set nc %d\n",nc);
      
    }
    else if(compare_char(keyword,"#chitarget")==1){
      fscanf(input,"%le",&likeness->target);
      
      //printf("set target %e\n",likeness->target);
      
    }
    else if(compare_char(keyword,"#Nw")==1){
      fscanf(input,"%d",&likeness->gwroom);
    }
    else if(compare_char(keyword,"#grat")==1){
      fscanf(input,"%le",&likeness->grat);
    }
    else if(compare_char(keyword,"#chimin")==1){
      fscanf(input,"%le",&likeness->chimintarget);
      
      //printf("set chimin %e\n",likeness->chimin);
      
    }
    else if(compare_char(keyword,"#Ng")==1){
      fscanf(input,"%d",&likeness->kk);
      
      //printf("set kk %d\n",likeness->kk);
      
    }
    else if(compare_char(keyword,"#resumefile")==1){
      resumeswitch=1;
      fscanf(input,"%s",word);
      for(i=0;word[i]!=0;i++){
        resumename[i]=word[i];
      }
      resumename[i]=0;
      
      //printf("resuming from %s\n",resumename);
      
    }
    else if(compare_char(keyword,"#write_every")==1){
      
      fscanf(input,"%d",&i);
      likeness->writevery=i;
     
    }
    else if(compare_char(keyword,"#outputfile")==1){
      fscanf(input,"%s",word);
      //printf("%s\n",word);
      for(i=0;word[i]!=0 && word[i]!=' ';i++){
        likeness->masteroutname[i]=word[i];
      }
      likeness->masteroutname[i]=0;
      
      //printf("set output %s\n",likeness->masteroutname);
    }
    else if(compare_char(keyword,"#guess")==1){
      
      nguess++;
      if(nguess==1){
        guess=new double*[1];
	guess[0]=new double[dim];
	for(i=0;i<dim;i++)fscanf(input,"%le",&guess[0][i]);
      
      }
      else{
        buff=new double*[nguess-1];
	for(i=0;i<nguess-1;i++){
	  buff[i]=new double[dim];
	  for(j=0;j<dim;j++){
	    buff[i][j]=guess[i][j];
	  }
	  delete [] guess[i];
	}
	delete [] guess;
	guess=new double*[nguess];
	for(i=0;i<nguess-1;i++){
	  guess[i]=new double[dim];
	  for(j=0;j<dim;j++)guess[i][j]=buff[i][j];
	  delete [] buff[i];
	}
	delete [] buff;
	guess[nguess-1]=new double[dim];
	for(i=0;i<dim;i++)fscanf(input,"%le",&guess[nguess-1][i]);
	
      }
      
    
    }
   }
   fclose(input);
   
   //printf("done with input\n");
   
   /*for(i=0;i<nguess;i++){
     printf("guess %d\n",i);
     for(j=0;j<dim;j++)printf("%.3e ",guess[i][j]);
     printf("\n");
   }*/
   
   if(resumeswitch==0){
     printf("about to initialize\n");
     likeness->initialize(guess,nguess);
     likeness->nsamples=1; //so that no Kriging is done when setting Kriging parameter
   }
   else{
    likeness->resume(resumename);
    nstart=likeness->npts;
   }
   likeness->write_pts();

   likeness->nsamples=nc;
   while(likeness->npts<nend){
     
    if(likeness->ngw==0 && likeness->nnodes==0){
     likeness->sample_pts(1);
    }
    else {
     for(i=0;i<likeness->nnodes;i++){
       //printf("going to try to sample from %d\n",i);
       likeness->node_sample(i);
       likeness->sample_pts(1);
     }
     for(i=0;i<likeness->ngw;i++){
       likeness->grad_sample(i);
       likeness->sample_pts(1);
     }
    }
   }
   
   likeness->write_pts();
   
   printf("after everything\n");
   printf("kk %d\n",likeness->kk);
   printf("ns %d\n",likeness->nsamples);
   printf("target %e\n",likeness->target);
   printf("chimintarget %e\n",likeness->chimintarget);
   printf("spent lingering %d\n",likeness->spentlingering);
   /*for(i=0;i<dim;i++){
     printf("%s min %e max %e\n",likeness->pnames[i], \
     likeness->CLmin[i],likeness->CLmax[i]);
   }*/
   
}
