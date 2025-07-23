function [ch,ch0,namech]=f_oszi_read(pathSequenceListAfterExecutionComputerReadable,pathOutputListComputerReadable)

%--------------------------------READ IN TIMEINGS
file = fopen(pathSequenceListAfterExecutionComputerReadable);
datain = fscanf(file,'%e %e %e %e %e %e %e %e %e %e %e',[11 inf]);
fclose(file);

file = fopen(pathOutputListComputerReadable);
ii=1;
temp=fscanf(file,'%e %e %e %e %e %e %e %e ',[8 1]);
while (isempty(temp)==0)
    datain0(1:8,ii) = temp;
    namech(ii)=cellstr(fgetl(file));
    temp=fscanf(file,'%e %e %e %e %e %e %e %e ',[8 1]);
    ii=ii+1;
end
fclose(file);

%-------------------------------PREPARE DATA

ch=datain';
ch0=datain0';
%ch0(:,3)=ch0(:,3)+1000000*ch0(:,2);%code analog and digital in channel
ch0(:,3)=ch0(:,3)+1000000*ch0(:,2) +(ch0(:,3)>=10000).*1000000; %shit version
ch0(:,3)=ch0(:,3)-(ch0(:,3)>=3000000).*1000000; %shit version
ch(:,3)=ch(:,3)+1000000*((ch(:,2)~=1)+1);%code analog and digital in channel
%ch(:,1)=(1:size(ch,1)).*20;%artificial timing<<<<<<
ch=sortrows(ch,3);


end