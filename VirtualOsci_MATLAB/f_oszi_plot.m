function f_oszi_plot(ch,ch0,namech,mintime,maxtime,const,handles,excludevector)

%------------------------------PLOT
set(handles.f,'CurrentAxes',handles.ha)
hold off
plot(0,0)
set(handles.f,'CurrentAxes',handles.ha2)
hold off
plot(0,0)
colors=['r','b','k','g','m','c','y'];%see colorspec


chlprevious=[-1,-1,-1,-1,-1,-1,-1,-1,-1,-1];
plotii=0;
actualplotii=0;
for chl=ch'
    if (chl(2)~=-999  && chl(3)~=2000777)%&& chl(2)~=6    %ignore all other kinds
        if(chl(3)~=chlprevious(3)) %finish old start new channel
            %finish and plot the done one
            if (plotii>0 && sum(plotii==excludevector)==0 ) %just to exclude plotting in the very beginning
                plot_t=[plot_t,max(ch(:,1))];%last point
                plot_v=[plot_v,plot_v(end)];

                maxplot_v=max(plot_v);
                minplot_v=min(plot_v);
                if (chlprevious(3)>=2000000 && (maxplot_v-minplot_v)~=0)%normalize
                    plot_v=(plot_v-minplot_v)./(maxplot_v-minplot_v);
                end

                set(handles.f,'CurrentAxes',handles.ha) 
                hold on
                if((mod(actualplotii,2)))
                    fill([0 maxtime maxtime 0],[actualplotii*const.separationfactor actualplotii*const.separationfactor (actualplotii+1)*const.separationfactor (actualplotii+1)*const.separationfactor],[0.9 0.9 0.9],'line','none');
                end        
                h=plot(plot_t,plot_v+actualplotii*const.separationfactor,colors(mod(chlprevious(7),length(colors))+1));
                if (chlprevious(3)>=2010000 && (chl(3)-mod(chl(3),20))==(chlprevious(3)-mod(chlprevious(3),20)))
                else
                    set(handles.f,'CurrentAxes',handles.ha2)
                    hold on
                    [maxtemp,rowofchannel]=max(ch0(:,3)==chlprevious(3));
                    if((mod(actualplotii,2)))
                        fill([0 1 1 0],[actualplotii*const.separationfactor actualplotii*const.separationfactor (actualplotii+1)*const.separationfactor (actualplotii+1)*const.separationfactor],[0.9 0.9 0.9],'line','none');
                    end
                    text(0.01,(actualplotii+0.2)*const.separationfactor,['ch=',num2str(chlprevious(3)),' ',num2str(minplot_v),'-',num2str(maxplot_v),cell2mat(namech(rowofchannel)) ],'HorizontalAlignment','left','VerticalAlignment','Baseline','FontSize',7);
                end

            end
            if (chl(3)>=2010000 && (chl(3)-mod(chl(3),20))==(chlprevious(3)-mod(chlprevious(3),20)))
            else
                plotii=plotii+1;
                if (sum(plotii==excludevector)==0 )
                    actualplotii=actualplotii+1;
                end
            end
            %start the new one
            plot_t=-1;%first point of channel
            plot_v=(ch0(:,3)==chl(3))'*ch0(:,7);%extract start value from ch0
            chlprevious=chl;
        end
        plot_t=[plot_t,chl(1)];
        plot_v=[plot_v,plot_v(end)];
        plot_t=[plot_t,chl(1)];
        plot_v=[plot_v,max(-42,chl(8))];
        if (chl(2)==4)
            plot_t=[plot_t,chl(1)+chl(11)];
            plot_v=[plot_v,chl(9)];
        end
    end
end
set(handles.ha,'ytick',[],'xlim',[mintime,maxtime])
set(handles.ha2,'ytick',[],'xtick',[],'xlim',[0,1])
%xlim([mintime,maxtime])

end