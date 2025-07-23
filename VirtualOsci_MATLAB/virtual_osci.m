function main_oszi()

clear all
close all

rootpath='C:\';
const.separationfactor=1.5;
tmp=get(0,'ScreenSize');
const.screensizex=tmp(3);
const.screensizey=tmp(4);
const.buttonsizex=150;
const.buttonsizey=25;
const.descriptionsize=300;


handles.f = figure('Position',[0 0 const.screensizex const.screensizey-50],'color','w','Name','That_s what_s going on in Control');

hzoomtime    = uicontrol('Style','pushbutton',...
    'String','Zoom in time','Position',[2*const.buttonsizex,1*const.buttonsizey,const.buttonsizex,const.buttonsizey],...
    'Callback',{@zoomtime_Callback});
hzoomouttime    = uicontrol('Style','pushbutton',...
    'String','Zoom out in time','Position',[2*const.buttonsizex,0*const.buttonsizey,const.buttonsizex,const.buttonsizey],...
    'Callback',{@zoomouttime_Callback});
hexcludechannels    = uicontrol('Style','pushbutton',...
    'String','Exclude Channels','Position',[4*const.buttonsizex,1*const.buttonsizey,const.buttonsizex,const.buttonsizey],...
    'Callback',{@excludechannels_Callback});
hshowchannels    = uicontrol('Style','pushbutton',...
    'String','Show certain Channels','Position',[5*const.buttonsizex,1*const.buttonsizey,const.buttonsizex,const.buttonsizey],...
    'Callback',{@showchannels_Callback});
hshowallchannels    = uicontrol('Style','pushbutton',...
    'String','Show all Channels','Position',[4*const.buttonsizex,0*const.buttonsizey,2*const.buttonsizex,const.buttonsizey],...
    'Callback',{@showallchannels_Callback});

handles.ha = axes('Units','pixels','Position',[const.descriptionsize+5,3*const.buttonsizey,const.screensizex-const.descriptionsize-10,const.screensizey-5*const.buttonsizey]);
handles.ha2 = axes('Units','pixels','Position',[5,3*const.buttonsizey,const.descriptionsize,const.screensizey-5*const.buttonsizey]);


%init
%set([f,hsurf,hmesh,hcontour,htext,hpopup],'Units','normalized');


[ch,ch0,namech]=f_oszi_read([rootpath,'SequenceListAfterExecutionComputerReadable.dat'],...
    [rootpath,'OutputListComputerReadable.dat']);
endtime=max(ch(:,1));
starttime=min(ch(:,1));
mintime=starttime;
maxtime=endtime;
excludevector=1000000;
excludevectorold=1000000;
f_oszi_plot(ch,ch0,namech,mintime,maxtime,const,handles,excludevector);

    function zoomtime_Callback(source,eventdata)
        set(handles.f,'CurrentAxes',handles.ha)
        [mintime,tmp]=ginput(1);
        [maxtime,tmp]=ginput(1);
        if(mintime>maxtime)
            tmp=mintime;
            mintime=maxtime;
            maxtime=tmp;
        end
            set(handles.ha,'xlim',[mintime,maxtime])
    end

    function zoomouttime_Callback(source,eventdata)
        mintime=starttime;
        maxtime=endtime;
        set(handles.ha,'xlim',[mintime,maxtime])
    end

    function excludechannels_Callback(source,eventdata)
        excludevector=1000000;
        f_oszi_plot(ch,ch0,namech,mintime,maxtime,const,handles,excludevector);
        ii=1; tmp=1;    
        set(handles.f,'CurrentAxes',handles.ha2)
        hold on
        while(tmp>0)      
            [tmp2,tmp]=ginput(1);
            if(tmp>0)
                excludevector(ii)=floor(tmp/const.separationfactor);
                set(handles.f,'CurrentAxes',handles.ha2)
                plot(1,(excludevector(ii)+0.5)*const.separationfactor,'xr','linewidth',5,'markersize',15)
                ii=ii+1;
            end
        end
        hold off
        excludevector=unique(excludevector);
        f_oszi_plot(ch,ch0,namech,mintime,maxtime,const,handles,excludevector); 
    end

    function showchannels_Callback(source,eventdata)
        excludevector=1000000;
        f_oszi_plot(ch,ch0,namech,mintime,maxtime,const,handles,excludevector);
        excludevector=1:100;
        ii=1; tmp=1;    
        set(handles.f,'CurrentAxes',handles.ha2)
        hold on
        while(tmp>0)      
            [tmp2,tmp]=ginput(1);
            if(tmp>0)
                showtmp=floor(tmp/const.separationfactor);
                excludevector(showtmp)=0;
                set(handles.f,'CurrentAxes',handles.ha2)
                plot(1,(showtmp+0.5)*const.separationfactor,'ok','linewidth',5,'markersize',15)
                ii=ii+1;
            end
        end
        hold off
        excludevector=unique(excludevector);
        f_oszi_plot(ch,ch0,namech,mintime,maxtime,const,handles,excludevector);     
    end

    function showallchannels_Callback(source,eventdata)
        excludevector=1000000;
        f_oszi_plot(ch,ch0,namech,mintime,maxtime,const,handles,excludevector);
    end
end