**Overview**

&nbsp;

OpticsFoudry's control system can be accessed through three types of software, listed below. This repository contains ControlAPI and Control.exe. 


&nbsp;

1. **ControlLightAPI**  
    A simple API giving the most direct access to the hardware. Tested with Python, Visual Studio C++ and Qt Creator (with MinGW).  
    ControlLightAPI can be configured through commands or through a json file. See repository [ControlLightAPI](https://github.com/opticsfoundry/OpticsFoundry_ControlLight) contains the ControlLightAPI.

2. **ControlAPI**  
    A fully featured API, having functions such as complex sequence assembly with "GoBackInTime", "Wavefunctions", and sequence cycling with individual command updates in the background. This is for example used for the [AQuRA clock](https://www.aquraclock.eu/).  
    This API's DLL has been tested with Visual Studio C++ and Qt Creator (with MinGW, see repository [OpticsFoundry_Control_Qt](https://github.com/opticsfoundry/OpticsFoundry_Control_Qt)). The API can also be accessed through TCP/IP, and we provide a Python and a Qt Creator example for that.  
    (If you need this API directly from Python, then tell us and we can create such a version within a few days.)  
    ControlAPI can be configured through ASCII files, a simple directory setting txt file, a hardware device configuration json file and a user output definition json file.

3. **Control.exe**

This is a fully featured experiment control system, based on ControlAPI.

Control.exe can be configured through code and/or through configuration files (the latter in the same manner as ControlAPI, on which Control.exe is based).

A somewhat outdated introduction and manual is available on [www.strontiumBEC.com](http://www.strontiumBEC.com) -> Control.

&nbsp;

&nbsp;

See Section "Installing Visual Studio 2022" below. Pay attention to always **use the same x64/Win32 Debug/Release settin**g for the DLL compiler and for the compiler that compiles the program that uses the DLL.

- **_x64_ or _Win32_** (i.e. 32bit or 64bit AMD/Intel code). For most situations x64 is better. The one situation in which Win32 is better is when editing the Control.exe software, as the compilation of Control.exe is vastly faster in Win32 than x64 mode.
- **_Debug_ or _Release_**: Release is quite a bit faster than Debug, both when compiling and when executing the program. Debug enables one to see Visual Studio debug information (even when using the DLL in Qt with MinGW).
- **_DLL or EXE:_** for ControlLightAPI you choose by modifying CMakeList.txt as explained in that file. For Control.dll vs Control.exe, in the Solution Explorer (in the Visual Studio GUI) you right click on "Control" (not " Solution Control" ) and select it under Configuration Properties -> General-> Configuration Type.

Control uses Microsoft Foundation Classes (MFC, or also called Afx). When installing Visual Studio C++, you need to select it as an extra option, otherwise it won't be installed.

&nbsp;

To install the Python library control_light_api systemwide via pip, use a command prompt, navigate to ControlLight_DLL_Test_Python_for_pip and run "pip install .". The DLL and PYD file need to be in the subdirectory "control_light_api" and the pyd file needs to be renamed \__init_\_pyd.

&nbsp;

&nbsp;

&nbsp;

1. **Control.exe**

&nbsp;

See section "Installing Visual Studio 2022" below.

&nbsp;

This is a full-fledged experiment control system. It's also the basis of ControlAPI.

&nbsp;

The code is in  
_Control_Firefly_AQuRA_

and you load it into Visual Studio C++ 2022 by double clicking on _Control.sln_.

&nbsp;

To compile Control.exe and not Control.dll in Visual Studio, go to Solution Explorer (Ctrl+Alt+L), right click on "Control" (not " Solution Control" ) and select EXE under Configuration Properties -> General-> Configuration Type.

You can select Win32 or x64 and debug or release in Visual Studio's dropdown menus. I recommend Win32 debug, as it compiles much faster in debug mode then x64. It's thereby easier to change CSequence.cpp and quickly resume operating the experiment when needing to make changes to CSequence code blocks (which is usually not all that often). If you came from DLL and switched to EXE, you might have to select exe in the dropdown menu marked below as "Local Windows Debugger", see little white downwards arrow.

![](data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAzkAAABWCAIAAABjOs9cAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAAEnQAABJ0Ad5mH3gAACcmSURBVHhe7Z0LdBTXff9XD/SW0OqNVg9khHjEYEMwWUVNhGtxXKxQ/E8VQozbtIi4f58YElc+2P8mjqraOQGfqriRWzcOcpoEDip/1XZoMX4otsnfhLV9DAZsHkYCvYUegCUhBEIS/9+de2fmzu7M7Kx2ZneFfp+zB+5jHvfe+c3c7/zuvaOw/Px8G4IgCIIgCBKShLP/EQRBEARBkNADtRqCIAiCIEjogloNQRAEQRAkdEGthiAIgiAIErqYqdXCw8OXLFmycuXKxMREloQgCIIgCIL4QURycjIL+sesWbMWLVrkcDhiY2MzMzMjIyNHRkYmJiZYNoIgCIIgCOI75mi1sLCwwsLCvLw8Go2IiEhJSUlISACtBoqNJiIIgiAIgiC+4oNWmz17dnx8/OjoKItzZGVlLVy4kEVE4uLiMjMzQcYNDg7eunWLpSIIgiAIgiCGMaTVwsPD77jjjqVLlzocjvHxcdBeLEPAbrcvW7YMNBmLc0BiSkoKiLyenh6WhCAIgiAIghjG+9qCmJiYFStWFBYW0ujChQsLCgpoGIiPj7/rrrtAzLG4GmlpaSyEIAiCIAiC+IIXrZaamlpcXGy321lcoKioiI54RkVFLVu2LDo6mqYjCIIgCIIg5uJFqy1ZsgQEGYtw5OfnL1q06O67746Pj2dJplJWfZCnukxKpEES3lXJXH0BoLByFy2JiPrJyWasgIrSWgE5vsfhpUSrzz4VhDLJBKqhKMqTT8V2Qq1J3WzSSJX4KkBYaxfejE3E4yYyehl0ihpElBYlEmoG4nfDhWDjm9vyAaugh/0HtF2FRgsh65QgBVOWi6Qo20baZgq1sOhp5g+qRYKq6RuEF62m4zPLy8tz87dRbty4wUL+0dW4ZY1ITRNJaaqRgsHAVUsLI7C5vpkla6Eordfr4DtNh102Z4nygpeVOG2uw+ScQW4rd4THVJWNa8FaV24+bRCrG0pCsqgtjbaKOp9PM/Umta5Skk0KVfL6RAq+VShuotqOirrQ7D+MIDSmUA0XVy+fWtdKa7cVlhbburocxaVWnSBomNDywcJ4J2K2bQT/3tfAoyMrzM+12RR2S1JCsl8LKN7nq1EGBgauXbvGItrANh999BGLIJbiKdZkqRZaFFY+VWEDocTfY0013uWuVTTXb2+8zTqx5voGT+ke6sCDd0tjl7Nquqq1kKZsY4XtyPYGl6NiIzYvEsK0dnbZxBd3AN4xHF3wkpEzlyXQlM5WFpuxGNVqY2NjR48eHR8fZ3E1RkZGPvzwQ+s+qKbzpsG5l617UVUHSsXYVSlZFyCVlmxQ5bQ5iA/BVCeCu1jjpZpbW3m2DymVXBhFzGynMfQaDleDpjKzvqF0oHUlZ5ZOybWVohhSOSlaJsfvDnsHslLi80xZUu5yulWBhzYBQWnGFuOuMbVaFZCz5BzNmgJ8jcoUOZajXgu+QIVCzELDgIdB15FDzU17QAwrnBZCS0gFdGs9ES5ZghSYKyfZHDaTKyUibqPeCBbDV8KtVXWyZJTXKAAIpZJKw2KqtqHWniRN/Wp6VAQS+CpxR+Nbw4cDmkbzoSPcmzMRZq6Ghg7uqTA3x0FsmQS5WmgXVVlYt6eZasXJ9nIjKGL0NCwSQLhysqoZ1WoAiLDTp09rDXF2d3eDmDNrANQnoFZ1xUfY+BYZVwlc08JVrcoVh9YacqoqHCyDg7hta11sBM5U961SrMmOYjdU24fsK73MEJUnHwluDTO9c4a9fdY1lCodbUw95m4o6RRPSdqqokMeWcytUr1RtUxOsfuWxs6AVaqsusqpo4e9YMSMrYJ7q9a5kaHbesq2XcgwNNwbxBqp1wJSpQLVHoEEaw2DSTXaESrEGuCsYm1Z63JUPCV2faU5YqHVW1jxxGBdan0zqQZjS2MXqY5QF51LaR2Ku0958+pkyXhcowAg+PidG4SLwMYfapo8bUOnPVWupreK6LfGFA7oH4JYE91oRJh1tvLGRjoQ6WGtRNWSde59rYoHrjc0hurl9kGrBRj6UiHgeVfxEGd/43axm/J4kTQNZxUrDsCULnkicqeugfsroPAmJjw81YxKo32gjxRfZqAarsbGLnYkUinrHM5ghawNvVxWCymrrqvg2+rIHrF1hLaivQ2BPkg9rUnL5AQPYq24e3N9vdX3uGSToNSk8/pMUM24ua2DhXRvZFetOGbO925aBLFGOrUQO5ymejHbKkj9mSdCRaxJTUNKJ3aSzfU1YqkEZ6csy0TgaSO5PzyfNlRosAPrXkqrEE6qfvPqZLlh8TVS6URIYWxkoFrZaAr02lP1aupWxEtr+H5AfyHPAPFKMcuVjQ1SNN/11Yqqc+9rVzzwvSFvCQJVoBAZ6pfbB60WHR09f/58rdUG2dnZd955Z2RkJIv7DX2pENDthMil5HTdQeiGWY7J8NNCab9BPFkKwU98BAFFtmhtqabRPvLLDDHOw/VtHfRIxDjZY94Kmus3kwYk7+CBRmoF4a1LNir5EpK2UlxQ4Sni0WtpNSlJ13qqWINsk1s6N0xV/QbXjIWZxAStVhVQPC9leadBEGukVQtBMsHjORAvKIWVG7h7WDizQtoqTVw2b/k9ius3OHSeNgqlpn8prYKcVOPm1cniCcA18uxEAKKN4bzannHd9lS5mvoV8dYaPh/Qf4jTQRBNUDZmuUQ9ke5Jc7iIoFJUnXtfp+KkgoHtDRWrTAiypiTlVLncRrVaRETEsmXLYmJiWFwNu92+YsWKYHxuzb3euuLOPObmBOARpI/4+NSUagKq7SPcf3B/gHGTnpAcCYyVxMw1TnKvyA+CICKrfxOWNQTJ5DSh74j63iYNgmvGZMxDMjiTWjXIN6b67UZeUoRRF6sFG3kW8M968qhnGksbMkeHG3ZR90NqPW2USo0SajeIEQJ3jdygekLXgeNTe5peEatbhog16CQ4ocW6J2JqPjm2pnjvB6o3NIrK5Taq1TIzM2fPns0i2sA299xzD4sEhiBKAY9TB6GPoI/PMm2ppt0+xDxz82FXm2COcCRnSSUcSPnq4TfkrXGaLEbzbCvu4SGj1aRBNEWG2qXzapTBNGNunp1u69F3Xobn+zFFKncQa6RvA6TLI0tfp6SpDSL0bvKgBAFO6UWsETuXh5lJTBXVpw2ZUqBUakG5ETxPKt28OlmeBOIaKSFSt6OWzBIUp1y5MbX21KqIT63BY2XLQKEcxRs3coZFfF7Oko38q5wRPGon3/u6FQ9Qb2gEjcvtRavpLPzs6+tTXfJp0ddxNaFSQJ5rWVhZHaibjJ5aur9Iv0NDaih6GxMhj8+KKsX8KwU67QN3A9mVGqdgIrnFeu65qULmDDirlCt1tLGqoQzg2VbEX7DHvT20mtQtvbCyUtzC+kqRooqXjjz3RHGsb5QCPpmxiZAhN26enY6hAvKXPUj5uthF0agpee4Go0aAVi3KqqUkQO4ATDcMQaq59W60Pby9Londg2BKQsgTQaxt4B8S9HIofWr6l9IiPE8q3bw6WTxa18haxMLQeWu8WpNsw9f21K+Iwdbgsb5lBBN1OnkfGtiazen0dQyS1k713teveKB6Q+9oXG4vWu306dOTk5MswtHb23v8+PGPP/44KAs/3WiqWSNoAUpdcadP19Yw0hkEaEM21QhrpmhSyWF46RA29UCYHmiR/5iYtE1vnpR2+wi7SvcC3C42hzXGKSxs6lAMwXeozoW3sqGMAAVlnn4CGRRSHS/ValJFel2xjT53rKuUVAShQcVxEXj/lUqhY5QyRs3Yf+QCQ5FzGtYoxnK0DdXW1VhLJuQRiLwTL4pWTSE9UDXyQL0WrZ2iVRGjYpW2wDDKNlaoOCJITygvbVOBCgXaXk/ZtmuvxYAi2xxyB0e7QuhYhD0FhLroXErrgJNq3bw6WTKq18hcpCYRgKbinZIKmaG0Dd/a01tFDLUGTwBaRjBRuTMi0J7NZ2Wo8zTTrXjAekPvqF7usPz8fJavQVpaGmwj/f11kG7nBW7dugXRzMzMoqKiuLg4mqvFm2++yUIIMj0pqz64odPbQw0JSfDamQY2JYIEA+/z1QYGBo4fP97d3U2jHR0dLS0tVKgBvb29Z8+eVfW9SQwODrIQgkxTlGvrkOkEcf5M/eNzCAfeBggSHLz71SSysrLCw8Ml0cYzd+7cBQsWsIgS2P706dP6f/AAQUIX8mlFMuXBJY0xIiGPeNEoeOlMgDUptiWCBAMftJoOYWFhRUVFoNhYXGBgYKCjo6Ovr4/FEQRBEARBEB+JSE5OZkH/GBoaioqKSkpKgvDY2Fhra2tzczMk0lwEQRAEQRBkCpim1SYnJy9fvmy32yF86tSpzs7OiYkJmoUgCIIgCIJMDXPGQBEEQRAEQRArMPp3CxAEQRAEQZDAg1oNQRAEQRAkdEGthiAIgiAIErqgVkMQBEEQBAldUKshCIIgCIKELqjVEARBEARBQhfUagiCIAiCIKELajUEQRAEQZDQBbUagiAIgiBI6IJaDUEQBEEQJHTBvzGFILc/8+bNY6EZQ0tLCwspmYFNMY3QumoIMsNBraZCXl5ee3s7iyDBBi+HEfRbCQTKtWvXWGQGEBcXp6PVZlRTTCN0rhqCzHBwDBRBEARBECR00dNqZWVl6enpNJyUlOR0OmkYiYwg7ea8K7f13b/jUxAEQRAEQcxFT2F8/PHHg4ODNDwyMnL69GkaNkJERERiYqLdbk9NTQWdFxsbGx5+m6gZkGV7d67fVLGcxW22n3z/vp1/v4ZFQoaEeffYlz3AIrc1KSkpDoeDRUTi4+MzMjJyc3Ozs7OTk5PBIFmGxYSFhcHp4KR5eXlQADB+liECRYUCswiCIL4QERWxYN2ChKwEFkeQmYGX+WrQ8YDkiouLGxgYGB8fZ6m6QKc4e/Zs2GtiYmJsbOzWrVtwkJiYGMj64osvhoaG6GbWAQWAAkdGRoI6vClw48aNyclJlm0ArxOkQKj95Pur6n77YcX9SxrfPPnoQ/dsfebA64c+h6yFCxcWFBTQzdy4cOHCmTNnWMR6MlZ9N+ebT1/41dYrx15nSWqAnkv/+ndZRJeJGyNdr/70eq+hCSXhs2Iyyx6JiE9mcQ1u9LcOvL/n1sQEi6vh9XJkZWUlJCQ0NzfTaFRUFNVnV69eBQsESwDdBtYIB4EUuo1FgOGBOoyOjoZ3GzC8WbNmQcEg3NHRAUZItyksLIRiXLx4kUbNAuer8eB8temIkflqWcuy7n/+/vHR8aMvHT39ig/uAwQxF3htKFxT2PVB19WL1nYrAIiZCOjSWEwN2AJUF2gv0FjQ27FUbaBfhI4TuqjLly+DMhsVGR4eht3hOLGxsdevXwcBx3YwFVBpdrs9PT0demuIgkaEksBJk5KS4IxSZ+kV2EVyKHrivCu3//K1tu5B1ycd2186lJkW/9m5vpa2y0mJMf2XR3784x9D9wzFSFPicDhWrVr1xhtvsKNYSWRCStKir0Wl5kUlZ6aVfGe0+4yOwMr79rPQTKPtx8eH+uwr/nyk9ZOx/vMQ5n+2sPDZS/705uDFOav/dvDk78dHrrCdtYlMtEcmpo4P9+v/khaXxmTMGzn/kU3bJPQvBwANDlccTA7C8FYwf/58MLa2trbe3l7Y8cqVK/CmAZaZk5MDdggSiu5lOlCGoqIiUGbnz5+/dOkSPTWUCswPbgq4g+jbTkpKCihI01WjfivBSa2reAgCNz40PosomWlNMY3QuWoSCXMSCv+sMHxWuMPpAN3W+0nv2NUxlocgASRzaeaqmlUL1i64ee3mwOkBlmoeoL6g25qcnAQlk5qa6kWrgcSBd1Dob4wINTj0nDlzoB+CjsrzaQgpcCjoVkGuQX/GUs3DRJmo3+2937C54s++lJ2eDELt5vjEtdGb//YPa9c/8KV5eSmNb35WXl6+b9++99577xMl/f39K1asOHjwIDuKZdiXPVC0dbd9xdqwiMiI2CTQbWOXOofPHmbZHqQ6vwVCrff3vxy5cDTjvu/1HNj5xbEDEOZ/E6NX7V/+xrnnvx3rWJy95jF9uRYRHR9/x5cj41Mmrg1p/caH+odO/eFa+6dXzx9NLf6LWYkZI63H2P4eGNdqINSo16q1tZW3QLjoYAawTUZGBmxm0atCQUEBvA/AqXknLoTBGuPi4iS5hlotAKBWm44Y12osnJVQ9I2i8WvjVvSUCKIPNcUT/3Hirk13Za/M7j1m5msD9GXwSKdKCboPiJq5tgBkH/SC0DNJfRXIo+joaBoGIB1kHHSZ0LmyJJMAmQh9IXSBfX19IMhYqghoxN7e3oiIiLS0NJbkB3Pv/Wf4/fe7px9cvSg9Jf6vHvxy9c/fgZQNj++jQ71BJCZz3ty/3tl/uOHkj4rb9/4oIib+yrHXu/+nlmX7yeREx76nr/Wcy/3WP7IUNfIe2lH0wwb934InXp37Nz8HNXnzSvfgyaaoVPfZZlNAEmptbW2qaqynpwes0aJrBK8KYNXd3d0szgGFgSJBwaB4QbcQiYWP7f2Dkr2PLWR5upQ/Z3TLaQFU5w/PlbOIAElR1lDahgTctvZE2IiiOAyX7vUYiEEiYyNX/mDl/f9yv68z2MqqDx6sLmMRs4GD76pkglLC84wkRbmdtA0J+Fi+wspd1tVIgpxFiWdNVVFtk9uAcwfPvfX4W+ER4Q/++sFF31zEUv0DhBMItYmJCeg7wsLC4uPj4fXStLUFtK9yE2rwCguvSjRKGR8fHxoagnQWNwlPmQhADaFUNGyuTPzJ9+979KF75qQnvvfb77V1X/7p4/fRpQZulbWCqk1/0vru30m/5qbH//5/l7I8my1l5YPjw5e79z83fvWy/ctrh5s/av3V1lsThiYaGmJy4nrnZ6CxWFSNWUnp/e/s+vRHX4EfRM//8lEapr+RC0ch9/Od65Pmf4XKNbqXn0DL6ws1AGwP1Dy8o7C4qcBh4bye7wkUXq4FwEiM4trxdZkdHetfmoEi4sAhl81ZytV6YUGuzeYoKZNVFklxHToAoQPboKG2kZA25aU2sVlJk4oNuvCx0vZHxHSX88nbSe8Gnay7s9b9xzqzekqLaDoMllbCianCfGJpxaWygCEprsNNEGqqWbNmTQ0JhSCuWiicSG1HRZ2Fsnc6MNI3cqj60MndJ5f/7fL7f+7za4Mb0dHRSUlJoFigpwASExNBrkHPpafVrly5AmqOzraBrfXHoWJiYuDoUl9FhRrs4jnWQ916IJto1H88ZSIFypCRkSHJNbNkYmREeHpKzNZnDnx0ois+LvzXrx77x399LyfTDjWqqKiA+paWln5LZPXq1Ww3k1i5NPt/3j37aPV/098//Pydh9cu3//vDzXsXA+5cXlLL3/02qKCFJB0Pa8/b7JQM4+xgbaWXz5qX/ZA/B0rWJJ/gH2Hh4eD+WVmZoK5UlgeB3k7sWY9MhxWdZ4AK0pWFhQMigeb8Z7mUAJ0yCP7upxPzjS11tLeZcstkITTwrISR1dXlyNP+usGQkq74U+0HtgmiTleB555YdsL4sKiAy/v61KoQcRvpuxgCxytnWBp+ZIyKywtFiwtZy5LoCmdrSw2TQBVuaWxy1k1o9UawRQHGwgtUFygVUZHR0HPgFwBGUOn2nvpt2BP6FpA2Xnt4UCpgPSjYSrU4ATQNcLuElSf3bp1C3o1E70LbjJRYmBgAIrEyzVTZOL4xOQWYdXn0MgN1yedkPJy49Fn/u0dEGorV65MS0tbtGjRcpGtW7cWFRXRHc3i4sDI0VM99Pf2H1u2PLv/j0c7IBwfG3Xt5IGx5vczUxO3/OVK2HIKQs2+vDzzvu+5/SCRZZsHyDUW8huQ4CCR6WxIHpbNAUYoWam5wGHBzCRLk2BFEYFCQlGhwCw7tDjzwm6Fk4kbJ3V3A8lZck65YnSUbCHrPnkAcO9j5YqcYHOm6TAnnIgwc+3e3cE1w7w8R9fhJkFncTWk1ZOaQdNNpqryzlzoYCHEVPx3sCnG+JT6g89iOXyS1xG+5kNHumQ3GhFmroYGsDTJ1zY3x9F15JCwnJ0bMaRjm9KZFKcRhkoFdlVKko+iWhFpjFVAEaOnISH+oF6qRGmub1C4DLlTux9BzpJzuLoCcjkIfFnKFDl6WWoFoMelO3Gbmor/DjbQTtSLBmGQNNBfgGihg6FeFBhooP7+/nPnznnt4eAE0tgT9EkQBQmVrmT27Nl0A69aLS8v72tKli5dKu3uBi8TBU0oQxcWSHLNRJkI4vfshUsbHt8HKvarX/3q2rVr33rrraeffnrPnj0/+9nPIECBLb1+2Wvjxo2CVcns2LFjyZIlLNsbn57r/dUrx3796rGE+KiJU6+H9x6PiZ6i6+jSB69EpWTHFSx3+0EiZLGNfCTOsSBu7nLpFx5j/lsvGHSzGixbJFn4ypqno1cVnywQgDcTeBnynBDJiqIECsyydfG1DCbAOZlAhrxUcpgN2/GjeTabY/1L1bYaIeORfTY+Rx2QOE/m7mOH2p335Hq/pieuW7fuZSXbtm1bsGABy/YZQayJbjQizNpbiENM9LWVlzptHRdUv7XjfJI1ww6XY321h1orf+5Jp2u35E3j0Dnm7YrZV00T6mBjER+B/ryuokMc4dvSmFsldeuKrC2N5B0dEktzjmwRk2wVdV40gCDWRDcaEWadrWRgVPS1lZWAVbS5P7YEnFVP2baT09S6HBVPMfkBuqMqt5GdvyGnqkK+rbQqwp9OOJ8ssaA8ZPgV9pQOWnuEZnmHcxmSUxeLrUIGSOVGcVTUsWoYai69Cnqpu0YBcjeUdJIMlcFlP3thHn8cbHRqmiSlaABECygcvU7d17UFoM9ogLoNoFPsVAKyj24AyglOT8OqtLe3t7XJfpfBwUGIag3C8jKRikIeUGYgpxITE+kGZmm1kpKSrVu3QvcPLQOX2Z8/qwrybvfu3Sxis504cQKiJ0+eZPEA0rN/x4Vdj6r+IItt5As3+luzHvjhHd97UfrFzimCRJYdQODNIScn5+LFi6CoWJIuPlkgAHbV1dUF9mbi6gFfy2ACssOnfNN6274aUWiQUTvO4eba8R2Wc+aFGsh5WMurJADKpIs71LYdLhqaIr8TYBEowZkzED179iyL+w6pNKscKSrxoYFYc1BfG5FVdLKaJ1KthEFNadRU9CE+3P6I2uQ2EMGg4XZ4mfZ2u2H6VdNipH/knf/zDov4RtnGClvjFqkjb67f3tjFtAxkOVy1YlZzfb0Qaq6vqWfaSvAtcSOcqjS3SW40EEqCDw3UE/O1EelEJ6t50tW4nZ6oaU+jKPfIEcRkMhRZK99W2hUBUSW69mB/V2NjFyszOZo4/CoKxqZ68ejeIBWjCKeWSwXF5RxurtrNLIcWaYOb102JXgW91F2rAEf2iBnumNsLj/SNvP/s+8M9wz69NoBQASRpBKoGdNv169epetPTaj6tLbhx44bUS0EYZFlCQoLqB0EiBLw66qCjoqvqYMtz587pd1GSTGSqUASOALtDVYeHh+kGXmViUABDee211yBw5cqVuro6ryaysCD997/+mw/+7yOqvxdr1sI20vqD7/6vu+leOkTG23Mf2l6w+UWdH2wAm7EdjHHu+W/zCwvo74sTb7PsQAGWSVce9PX1sSQD+GSBAFw7MDNzF3v6WgZ/ESbWE4hEcZDFBoyXeF+YYlzP63geOabCiUScd/4B3fzbbxMrggb5zW9+42+XL80rE6WaUEZBfMnrClRQusakSW/CCgSgxlbtMTgKOu6l9R07vC1QuC0x+aqp0fJGy/6/3t/xxykNMYMAcPNrEQkiaBmSpa6jCqXhtioDf4NRWl9AxAYd7iTqiYgveV2BCspiCUUi2yuSiW+LolMR2bUHG7kO17d1UOkmlYds4KzydZhQWCZBIKd2kMUGjDrO4WVTzMWT5Z0GOhX0UnfNAigbxR1fe2EdUheklv1TWXRitE+vDXT0j067B3EGOorXKnpazae1BXRYJzY2lkYlueY5ZAOJdEUei2vT0tLS1tYGCnd0dJQlqcHLRB6QqGlpaVDyS5cuUW+KQZkYFH7xi1+AkN+2bRtITJakzfo1i3v7r/3Xm2dUf6+9/XnTH89D4JW3To+NhUVHuU+i8iQyKT0ybnbPgZ06P9gANmM7eCNsVnT6vZuyHvih6i9+rvwXuqxGEmpgS5L/1SAGLVACNoYTmSvXfC2DX3Bzs9yWiQJT1BdwTBYyk71790Lfv2PHDjP+AgQb8+RUJRkYBfnm47oCJWde+A4ZHN0kuiPJ5LYnySLRmSjUKKZeNQXUnfb+z94P4KdxyeQnbrjNiLuYDUJyYkMQRyVlPq8rmJszpduKnY7IHXI64tYD6UZibKZcc/1mNmrqg2DjZtq5LRMFpricVaeCXuo+9QL41AurEjErYulfLV31zKr+T/tf++5rPr02gDiReijP8R8vE5tA6EQbW1sAYnB4eDg5OVnakso1Op1fIkr4agbIPoO9Znt7u9cuyk0mSqSmpvJCDTAuE/0BShInAlEoRn5+/pw5c2iuDqDrjZhIfOys1X8y/+S5no6eK6q/nv7BweExCERGhoeFTza+8Rnb0xvXez7X+bGNjBGdlp9YeM/1i+c8f5CbuPjrdDOr8UeoUYxYoAScwgq55lMZ/ICbYOW2OlIJt0iSeuDU5l5JEs3jUGaJN+j1zeryiRutZNOmEofkQ6MDo5t47eof5c/NWIeaAhOvmoRf7jQJ4pxhI4IMSVJ5ZgEkVx7VIzEDwJEcxRs3FguTwwTowOhGXu0YwaNIsoDRqQgVa7n5ZaXFNuF0INacJZUgFBU+JyLYyOpO/UFKkbLqKqerQWgH1YYSkebqETydfxSpGr5UUK/uPmKwF1aFutPySvLe+/F7U3htmJiYkEYIPfGuwAyuLQDoVzPS09Ol1XAg13h5CELNbrdDr+Mm4PzEUyZSIJEXar7KxKkBVQZZdocIpBQXF2/evLm8vLy3t5du4yerS+ZdvXajtdPLB76BO+dnvPX+ucuDAejm1fni6AHPH5VrAcB/oTYFLJJrliM4feRpVGRJqINbNbDwsee4wTz5yx5E3nXte1nYiQge0YtE0oUAdVFxU++5nNBBKKPTyfvQhIFRpzgmapjyx+R24hpn4WMPS+2EmIeZ7jQy5czBTUUvrHyqwta4h0gqt6zCykoWEmUB2dbQG4gwCEksTfahCQOjYGk+STVWJHGdgaCXaEi3IgBoQ0dFVQWVaoK4yS2WpWNZNe9MUxNTSsgYMCg1cSqf56mrObknf9mDFLeLFQlK4KjYSDO4aggtpVFBnSzdAliGP+40CapVqIzx9I7paTVf1xZALwVyBP7NyMgAVcQrxAjhD7rD0UA5gn4yveP0lInA9evXeaFmhUz0BHTtpxyQ0tjYuEXg2Wefpdv4SZo97lQzW6WhQ0JCTH727N++dpzFZxhBEWqUaSPXnE/SyWiEl/J2K4c5D2wTvtkqZpe0y5qla9+O9odpOpF38jqD70h7lB56ZJ84hQTSheWiQoYiJ3QgYg0qxgszItbc56R5p8VWIs3yI4tfxcYB+Pl/hND5bsk0xV93GpmXJSJ820H4Vpgw+kcgw5ui14xk1brEHeqKbaC0yPx4G5sa9ZRtu6ExUKozwNJ4YUbEmhFd5EZTjbCaUjj/wZLDWxrl20qnIux0UgGgPDaH7OVr7RR3I3upjx7y7VaX06AcZVQ0FBykU65oV2Nt5waaTuSdvM5gs7QHXw1I16qgTpZeASzCT3eaBGgYUCx07aMwXUv+ggQImzCdBYwgbkDZUI8a7EadUjRLB5BoIMtA20EYlBkAp4HdoRygqKADs6jjhFPQb3MMDQ1BsaWz0JIDdEwWikHTdcjLy2tvb2cRDcA+77333p6eHggvXrz4xIkTe/fupVk8r7766tatW48d0/xjl77SsHP9PUuyX/6vYyPXNA0iOioyIzUxJysxKz2+dOPLLFWDoh/852jnycFTh+aUP35h16MsVY2CzS/2HNg5e3FpbM6Sz//l2yzVA+mA2d94/PwvVQ6YvLw8Jmv+xdefv/OnH3z+/IbotJzo9Du69z/Hsj0wcjncCKJQk4AbAW4uMLzm5mbPj/+Zjn4rzZs3z+CHQgJA+XNkiSQnYswnLi6upUV9mllINQXCo3PVJLKWZd3//P0j/SMf/PMH/g56ItOKsuqDGzp50Smjk2Ud1BTP/u7s/G/Mv/D2hQ/rPvTfuQtCLSUlBfoLEE4Qhc4LgK4E0POr+bS2QAIODZqso6NjYGAANBOcdXh4+OLFi52dnRCAXLad2UDd4CxwClCK2dnZUOb09PQ5AvHx8VCXvr4+I0LNIIcPH3733XfPCNBmDQwjo2MTk7bVX53/YNmXtH4PrJp/z5I5Sxdk7tl/gu0WwgydOtT71ossYgag14Mu1AA4teRd4929Mx1uUhyC+MrFYxebtjWZMDsNmV6QgU42Kc4dnSzr8d+dxgOKC7qMmJgY6l0jEi08HP4lYf0Pg8F2qampycnJ0OuE5vJJT6BisbGxUeLfSACxCCX3qc/21ZHzwgsvXLp0CaQbi3M888wz5vrVFhSkr/m6ofH3G2MTv/ndJzruN0rRD/4zbNassf7WhKLiL47qzadJXl5+9fMjUelzb928qe9XgwPe6D4TlZoz2q2yIiEyISU8Om7sUmf6n27+/PkNV895GTuYgl8NXk1AnQdRqEmANdrt9suXL7O4ZYSyX418C1eeQOEKwAR79KtNR4z41ZCZA/ngLffcqOWGWnWyAonjK47+z/pNUWk8IGASExOh/5qYmJicnIQAUWz+fMT1dsVXcfDEE0+sW7eORZRAr1BZWTnldSUBIP1rG+3L/5yIilmRIMJYqhqgwG7dHAcBfOXo/v7/t4eleiAe0DsTN0badj8xftWLjpmCVpuBhLJWCzyo1aYjqNUQhALiLD4+fpYAREG06Wm1srKy48eP9wt/bCApKWnx4sUul7Hpk9McFAchBV4OI6BW40GtNh1BrYYgWujNV/Pp7xYgCIIgCIIgpuNlbYE0R21iYsLg2oLbAPTihBR4OYyArYQgCHK7gvPVEOT2Z9487i8NzAx0xkBZCAk9cAwUQVRBrYYgCIIgCBK66I2BIgiCIAiCIMEFtRqCIAiCIEjogloNQRAEQRAkdEGthiAIgiAIErqgVkMQBEEQBAldUKshCIIgCIKELqjVEARBEARBQhfUagiCIAiCIKELajUEQRAEQZDQBbUagiAIgiBI6IJaDUEQBEEQJHRBrYYgCIIgCBK6oFZDEARBEAQJXVCrIQiCIAiChC6o1RAEQRAEQUIX1GoIgiAIgiChC2o1BEEQBEGQ0CXM6XTSUE9PDw34QuHqzQVv73qbxZDbgm/e+wELIYjIK+9+hYUQBEGQQGKz/X93tok3lWvnlwAAAABJRU5ErkJggg==)

&nbsp;

&nbsp;

Control.exe configures itself from configuration files. It searches for "config.txt" in the folder that contains the executable, in the three folder levels closer to the root folder, and in any "ConfigParams" folder relative to those folders. If it finds a "_config.txt_" file that is empty, it will look for all other configuration files in the folder in which the empty "config.txt" file was found. If the _config.txt_ file is not empty, its first line (terminated by new line) will be used as config file path. If no _config.txt_ is found, the first argument passed to control.exe is used as configuration file path.

&nbsp;

The main configuration files are  
_ControlParam_ParamList.txt_  
_ControlParam_SystemParamList.txt_  
_ControlParam_UniMessList.dat_  
_ControlParam_UtilityDialog.txt_  
and are typically contained in the folder  
_\[Installation path\]\\Control_Firefly_AQuRA\\ConfigParams\\_

&nbsp;

All other paths and files are set in the configuration file  
_ControlParam_SystemParamList.txt_ (not _ControlParam_ParamList.txt_)  
e.g.  
_WorkingDirectory=._  
_HardwareConfigFilename=ControlHardwareConfig.json_  
_UserIOConfigFilename=ControlUserIOConfig.json_  
_UserIOAlternativeCommandNames=AlternativeCommandNames.txt_  
_DebugFilePath=..\\DebugFiles\\_  
_VirtualOsciFilePath=..\\OpticsFoundry_Control_AQuRA\\VirtualOsci\\_  
_DataFilePath=..\\Data\\_  
_SourceFilePath=..\\OpticsFoundry_Control_AQuRA\\_  
_CalibrationFilePath=..\\OpticsFoundry_Control_AQuRA\\Calibrations\\_  
_ParameterReferenceFileName=ReferenceParam_  
_UserParameterASCIIFileName=..\\DebugFiles\\OutputList.txt_  
_UserParameterFileName=ReferenceParam.dat_  
_OutputListFilename=..\\DebugFiles\\OutputList.dat_  
_OutputListFilenameComputerReadable=..\\DebugFiles\\OutputListCR.dat_  
and more (further down in the file).

&nbsp;

If the _WorkingDirectory_ is empty (i.e. "WorkingDirectory=" (no dot) instead of "WorkingDirectory=."), then the working directory will not be changed on program start (it typically is then the location of the exe file). If _WorkingDirectory=._ (i.e. a dot), then the working directory will be changed to the directory in which _config.txt_ was found, i.e. typically _\[Installation path\]\\Control_Firefly_AQuRA\\ConfigParams\\_. Relative filenames and directories (i.e. not starting with the drive, e.g. C:\\) are relative to the working directory ("..\\" means go one level closer to the root directory).

&nbsp;

Make sure the screen coordinates at which the Control window appears is on your screen, e.g.  
_MainDialogStartX=0_  
_MainDialogStartY=0_  
It's convenient to set this to appropriate values in multi-screen setups, but it can lead to a hidden window when switching screen setups.

&nbsp;

The hardware configuration file _ControlHardwareConfig.json_ is created by opening the _Control_Firefly_AQuRA_ folder (not the _Control_Firefly_AQuRA\\ConfigParams_ folder!) in Visual Studio code and running the Python script  
_ConfigParams\\ControlHardwareConfigFileCreator.py._  
The json file is then created in the _ConfigParams_ folder (verify that, if something doesn't work). After this json file has been successfully generated, the IO configuration file _ControlUserIOConfig.json_ is created by running  
_ConfigParams\\ControlUserIOConfigFileCreator.py_  
For convenience this Python script also provides an overview of your address usage in the file  
_ConfigParams\\AddressUsageTable.txt_  

Take a look at the _\__main___ function in the two Python files below to understand how one can define hardware and UserIO configurations.

The Python scripts check that all provided data is consistent, e.g. that a desired IO output actually is configured on a certain address, that an address is only used once, and that limitations on address usage are adhered to, e.g. that AD9858 addresses are a multiple of 4.

&nbsp;

&nbsp;

After running _Control.exe_ for the first time, you can adjust the size of the dialog window to your screen. You do this by opening _Control.rc2_ using the Solution Explorer in Visual Studio C++, opening _Dialog -> IDD_EASYDIALOG_ and dragging the dialog window to a suitable size. If your screen is very small or large, you might want to adjust the number of elements displayed in each menu column by editing _DialogElement.cpp_ and setting _MaxLines_ to what you need. (Btw, you can also edit the size of all other GUI components in the table of constants in this file.)

In _UniMessList.h_ you can adjust the maximum number of UniMess entries in the Measurement Menu to make them fit the screen:  
_const unsigned int MaxNrUniMess=76;_  
If you find menus with more columns then you can display, insert _NewMenu_ commands as appropriate, see manual below.

&nbsp;

&nbsp;

The following manual explains how to use Control.exe. (Please note that since the manual was written, Control.exe was made configurable through ASCII files, as described above.)

&lt;<ControlManual.pdf&gt;>

&nbsp;

Here an example hardware configuration file generator Python script:

&nbsp;

```py
if __name__ == "__main__":
   builder = ConfigBuilder()
   builder.RegisterSequencer(IP="192.168.0.109", Port=7, ClockFrequency=100000000, BusFrequency=2000000, DebugOn = False)
   analog_out_configs = [
       (24, False, 0, 10),
       (28, True, -10, 10),
       (32, True, 0, 10),
       (36, True, -10, 10),
       (40, True, -10, 10)
   ]
   for addr, signed, minv, maxv in analog_out_configs:
       builder.RegisterAnalogOutBoard16bit(StartAddress=addr, Signed=signed, MinVoltage=minv, MaxVoltage=maxv)
   for addr in [1, 2, 3, 4, 5, 6]:
       builder.RegisterDigitalOutBoard(Address=addr)
   for addr in range(132, 172, 4):
       builder.RegisterDDSAD9854Board(Address=addr)
  
   for addr in range(52, 84, 4):
       builder.RegisterDDSAD9858Board(Address=addr)
  
   builder.RegisterDDSAD9958Board(Address=21)
   builder.Save()
```

&nbsp;

&nbsp;

Here an example IO configuration file generator Python script:

You can specify ranges for analog outputs and DDS frequency outputs. Control will not send output values beyond these ranges to the outputs. This option exists to protect hardware, such as RF amplifiers, from destruction.

&nbsp;

```py
if __name__ == "__main__":
   builder = UserIOConfigBuilder()
   builder.add_new_menu(Name="User IO Analog Out")
   builder.add_static_text(Name="Spare Analog Outs")
   for i in range(8):
       builder.add_analog_out(Name=f"SpareAnalogOut{i}", Description=f"Spare Analog Output {i}", Address=24+i)
   builder.add_new_column()
   builder.add_static_text(Name="AQuRA Analog Outs")
   builder.add_analog_out(Name="MOTCoilCurrent", Description="Current in MOT Coils", Address=32, Units="A", Max=5.0, Init=0.0)
   builder.add_new_column()
   builder.add_new_menu(Name="User IO Digital Out")
   for i in range(8):
       builder.add_digital_out(Name=f"SwitchSpareDigitalOut{i}", Description=f"Spare Digital Output {i}", Address=4, Bit = i)
   builder.add_digital_out(Name=f"SwitchOscilloscopeTrigger", Description=f"Oscilloscope Trigger", Address=5, Bit = 0)
   builder.add_digital_out(Name="SwitchCameraTrigger0", Description="Camera Trigger 0", Address=5, Bit=1)
   builder.add_digital_out(Name="SwitchCameraTrigger1", Description="Camera Trigger 1", Address=5, Bit=2)
   builder.add_digital_out(Name="SwitchPMTTrigger", Description="PMT Trigger", Address=5, Bit=3)
   builder.add_new_menu(Name="User IO DDS Out Menu")
   builder.add_dds_ad9854(Name="UserIOTestDDSAD9854_0", Description="Test DDS AD9854 0", Address=132, FrequencyControl= "FrequencyAsBinary")
   builder.add_dds_ad9858(Name="UserIOTestDDSAD9858_0", Description="Test DDS AD9858 0", Address=52, PowerControl="PowerIndB", InitFrequency=77.0, InitPower=-10, MinFrequency=60.0, MaxFrequency=100.0)
   builder.add_dds_ad9958(Name="UserIOTestDDSAD9958_0", Description="Test DDS AD9958 0", Address=21)
   builder.add_new_column()
   builder.add_static_text(Name="AQuRA AD9854 DDS")
   builder.add_dds_ad9854(Name="CoarseSpectroscopyAOM", Description="Coarse Spectroscopy AOM", Address=136)
   builder.save()
```

&nbsp;

&nbsp;

&nbsp;

**Option for cleaner looking, 3x faster C code**

&nbsp;

There is one little twist to the story of configuring user IO through JSON files. To write the cleanest code and obtain the highest speed, one would like to write commands such as

&nbsp;

SetMOTCurrent(InitialMOTCurrent);

SwitchZeemanSlowerShutter(On);  
<br/>However, using only JSON configured outputs, one doesn't immediately obtain C functions named correctly. This is a limitation coming from the fact that C is a compiler. The directly available way of calling JSON configured user IO is through lookup functions "AnalogOut" and "DigitalOut":

&nbsp;

AnalogOut("SetMOTCurrent", InitialMOTCurrent);

DigitalOut("SwitchZeemanSlowerShutter", On);

&nbsp;

In order to also obtain C functions setting these outputs, we can auto-generate the required C code and include it in the source code of control. The advantages are 3x higher speed when calling output commands while assembling a sequence (a few hundred nanoseconds, instead of up to a few microseconds) and cleaner looking code. (The output speed on hardware is unchanged.)

&nbsp;

The C code is autogenerated on startup if the following is set in the configuration file _ControlParam_SystemParamList.txt_:

DoCreateIOListShortcuts = TRUE

DebugFilePath=..\\DebugFiles\\

After starting control.exe you will find the files

_\[DebugFolder\]\\IOList_shortcuts_auto_create.h_ and _.cpp_

Copy those into the source code folder and recompile.  
(If you let control.exe create those files directly in the source code folder, and if you inadvertently leave the DoCreateIOListShortcuts option on, Visual Studio will detect that the date of these files has changed and recompile control.exe every time you start it, wasting your time, even if the content of those files has not changed and recompiling is not needed.)

&nbsp;

Now you can use

SetMOTCurrent(InitialMOTCurrent);

SwitchZeemanSlowerShutter(On);  
just, as if you had defined those outputs in the "old-fashiond way" in IOList.cpp.

&nbsp;

The IOList_shortcuts_auto_create.h and .cpp need to be regenerated every time a new UserIO entry is added to the json file in order to define a C function accessing that user IO.

&nbsp;

This trick is not relevant when using Control as an API through a dll or through python bindings, as we access the outputs through the API, i.e. using  
Command("SetMOTCurrent(InitialMOTCurrent);");

Command("DigitalOut("SwitchZeemanSlowerShutter", On);");

which is converted automatically into the command lookups

AnalogOut("SetMOTCurrent", InitialMOTCurrent);

DigitalOut("SwitchZeemanSlowerShutter", On);

&nbsp;

Similarly,

_\[DebugFolder\]\\ParamList_shortcuts_auto_create.h_ and _.cpp_

need to be copied into source folder and give access to the initialization parameters of all outputs selected by the user in the initial parameter menus, by providing pointers to those parameters. These parameters can be useful if one wants to intermittently set an output back to its initial value, e.g. for the heating of AOMs.

&nbsp;

&nbsp;

&nbsp;

**Data acquisition using Vision**

&nbsp;

Control.exe directly supports the [Vision](https://github.com/opticsfoundry/Vision_AQuRA) data acquisition system and can easily be adapted to others.
