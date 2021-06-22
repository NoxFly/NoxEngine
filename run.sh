#!/bin/bash

# Author : NoxFly
# Copyrights 2021
version=0.4.5

# PROJECT MODE :
# 0: src/ClassName.cpp include/ClassName.h
# 1: src/ClassDir/ClassName.cpp src/ClassDir/ClassName.h
projectMode=1

# adapt these 3 lines for your project
fileExt="cpp"
srcDir="./src"
incDir="./include" # availible only on projectMode=1
binDir="./bin"
outDir="./out"
buildDir="./build"
cppVersion=17 # 98, 03, 11, 17, 20
cVersion=11 # 89, 99, 11, 17

os=linux

if [ "$OSTYPE" == "darwin"* ]; then
    os=mac
elif [ "$OSTYPE" == "cygwin" -o "$OSTYPE" == "msys" -o "$OSTYPE" == "win32" ]; then
    os=windows
fi

# if any mode is precised, then release is the default one
mode="dev"
# project folder's name is the default application's name
pgname=${PWD##*/}

#
updateUrl="https://gist.githubusercontent.com/NoxFly/69a73c1a9b31058c04217b9cebee2478/raw/run.sh"

# declare -a optionsList
optionsList=(
	"-h;--help;Show the command's version and its basic usage."
	"-V;--version;Show the command's version."
    "--patch;Download latest online version."
	"-g;--generate;Generates project's structure with a main file. Re-create missing folders."
    "--swap-mode;Swap between the two structure modes. Reorganize files."
	"-c [ClassName];Create class files (header and source) with basic constructor and destructor."
	"-d;--debug;Compile code and run it in debug mode."
	"-r;--release;Compile code and run it in release mode. If no mode precised, this is the default one."
    "-f;--force;Make clean before compiling again."
    "-v;--verbose;Add this option to have details of current process."
    "--no-run;Tells the script to only compile the project. Useful if the script is integrated in IDE's task."
)

repeat_spaces()
{
    for ((ii=0; ii < $1; ii++)); do
        echo -n " "
    done
}

getMakefileCode()
{
    echo -e "IyBtb2RpZnkgdGhlc2UgMyBsaW5lcyBkZXBlbmRpbmcgb2Ygd2hhdCB5b3Ugd2FudApDRkxBR1MgCQk6PSAtV25vLXVudXNlZC1jb21tYW5kLWxpbmUtYXJndW1lbnQjIC1XZXJyb3IgLVdhbGwgLVdleHRyYQpMREZMQUdTCQk6PQpMSUJTIAkJOj0KIyB0eXBlIG9mIHNvdXJjZSBmaWxlcwojIGMgb3IgY3BwIChtYWtlIHN1cmUgdG8gbm90IGhhdmUgc3BhY2UgYWZ0ZXIpClNSQ0VYVCAJCT89IGNwcApDVkVSU0lPTgk/PSAxMQpDUFBWRVJTSU9OCT89IDE0CgojIGRldGVjdCBpZiBjb21waWxlciBpcyBnY2MgaW5zdGVhZCBvZiBjbGFuZy4gTm90IHZpZXdpbmcgZm9yIG90aGVyIGNvbXBpbGVyCiMgQwppZmVxICgkKFNSQ0VYVCksIGMpCglpZmVxICgkKENDKSwgZ2NjKQoJCUNDIDo9IGdjYwoJZWxzZQoJCUNDIDo9IGNsYW5nCgllbmRpZiAjIEMgOiBjbGFuZyBvciBnY2MKCUNGTEFHUyArPSAtc3RkPWMkKENWRVJTSU9OKQojIEMrKwplbHNlCglpZmVxICgkKENYWCksIGcrKykKCQlDQyA6PSBnKysKCWVsc2UKCQlDQyA6PSBjbGFuZysrCgllbmRpZiAjIEMrKyA6IGNsYW5nKysgb3IgZysrCglDRkxBR1MgKz0gLXN0ZD1jKyskKENQUFZFUlNJT04pCmVuZGlmCgojIGV4ZWN1dGFibGUgbmFtZQppZmRlZiBQR05BTUUKCUVYRUNVVEFCTEUgPSAkKFBHTkFNRSkKZWxzZQoJRVhFQ1VUQUJMRSAJOj0gcHJvZ3JhbQplbmRpZiAjIHBnbmFtZQoKIyBwcm9ncmFtIG5hbWUgbG9jYXRpb24KT1VUIAkJPz0gLi9iaW4KCiMgY29tcGlsYXRpb24gbW9kZQppZmRlZiBERUJVRwoJVEFSR0VURElSID0gJChPVVQpL2RlYnVnCmVsc2UKCVRBUkdFVERJUiA9ICQoT1VUKS9yZWxlYXNlCmVuZGlmICMgZGVidWcKCiMgZmluYWwgZnVsbCBleGVjdXRhYmxlIGxvY2F0aW9uClRBUkdFVCAJCTo9ICQoVEFSR0VURElSKS8kKEVYRUNVVEFCTEUpCiMgLm8gbG9jYXRpb24KQlVJTERESVIgCT89IC4vYnVpbGQKIyBzb3VyY2UgZmlsZXMgbG9jYXRpb24KU1JDRElSIAkJPz0gLi9zcmMKIyBoZWFkZXIgZmlsZXMgbG9jYXRpb24KSU5DRElSIAkJPz0gLi9pbmNsdWRlCgpTT1VSQ0VTIAk6PSAkKHNoZWxsIGZpbmQgJChTUkNESVIpLyoqIC10eXBlIGYgLW5hbWUgKi4kKFNSQ0VYVCkpCgpJTkNESVJTCQk6PQpJTkNMSVNUCQk6PQpCVUlMRExJU1QJOj0KSU5DCQkJOj0KCmlmbmVxICgsICQoZmlyc3R3b3JkICQod2lsZGNhcmQgJChJTkNESVIpLyopKSkKCUlOQ0RJUlMgCTo9ICQoc2hlbGwgZmluZCAkKElOQ0RJUikvKiogLW5hbWUgJyouaCcgLWV4ZWMgZGlybmFtZSB7fSBcOyB8IHNvcnQgfCB1bmlxKQoJSU5DTElTVCAJOj0gJChwYXRzdWJzdCAkKElOQ0RJUikvJSwgLUkgJChJTkNESVIpLyUsICQoSU5DRElSUykpCglCVUlMRExJU1QgCTo9ICQocGF0c3Vic3QgJChJTkNESVIpLyUsICQoQlVJTERESVIpLyUsICQoSU5DRElSUykpCglJTkMgCQk6PSAkKElOQ0xJU1QpCmVuZGlmICMgaW5jZGlyCgppZm5lcSAoJChTUkNESVIpLCAkKElOQ0RJUikpCglJTkMgKz0gLUkgJChJTkNESVIpCmVuZGlmCgoKaWZkZWYgREVCVUcKT0JKRUNUUyAJOj0gJChwYXRzdWJzdCAkKFNSQ0RJUikvJSwgJChCVUlMRERJUikvJSwgJChTT1VSQ0VTOi4kKFNSQ0VYVCk9Lm8pKQoKJChUQVJHRVQpOiAkKE9CSkVDVFMpCglAbWtkaXIgLXAgJChUQVJHRVRESVIpCglAZWNobyAiTGlua2luZy4uLiIKCUBlY2hvICIgIExpbmtpbmcgJChUQVJHRVQpIgoJJChDQykgLWcgLW8gJChUQVJHRVQpICReICQoTElCUykgJChMREZMQUdTKQoKJChCVUlMRERJUikvJS5vOiAkKFNSQ0RJUikvJS4kKFNSQ0VYVCkKCUBta2RpciAtcCAkKEJVSUxERElSKQppZmRlZiBCVUlMRExJU1QKCUBta2RpciAtcCAkKEJVSUxETElTVCkKZW5kaWYKCUBlY2hvICJDb21waWxpbmcgJDwuLi4iOwoJJChDQykgJChDRkxBR1MpICQoSU5DKSAtYyAkPCAtbyAkQAoKZWxzZSAjIFJFTEVBU0UKCiQoVEFSR0VUKToKCUBta2RpciAtcCAkKFRBUkdFVERJUikKCUBlY2hvICJMaW5raW5nLi4uIgoJJChDQykgJChJTkMpIC1vICQoVEFSR0VUKSAkKFNPVVJDRVMpICQoTElCUykgJChMREZMQUdTKQoKZW5kaWYgI2RlYnVnIC8gcmVsZWFzZSB0YXJnZXRzCgoKY2xlYW46CglybSAtZiAtciAkKEJVSUxERElSKS8qKgoJQGVjaG8gIkFsbCBvYmplY3RzIHJlbW92ZWQiCgpjbGVhcjogY2xlYW4KCXJtIC1mIC1yICAkKE9VVCkvKioKCUBlY2hvICIkKE9VVCkgZm9sZGVyIGNsZWFyZWQiCgouUEhPTlk6IGNsZWFuIGNsZWFy" | base64 --decode
}

getHelp()
{
    echo "C/C++ run script v$version"
    echo -e "Usage : $0 [OPTION]"
	echo -e "\t$0 -g to generate project's structure."
	echo -e "\t$0 [-dev|-d|-r] to compile and run the program in [dev(default)|debug|release] mode."
    echo -e "All added options not handled by the script will be executable's options."
	echo -e "\n\033[1;21mOPTIONS:\033[0m"

    for i in ${!optionsList[@]}; do
        # get current command line as array (split ;)
        IFS=';' read -a option <<<"${optionsList[$i]}"
        # length of the array - 1
        n=$((${#option[@]}-1))
        # the description of the command is the last element of the array
        description=${option[$n]}

        # make a tabulation
        echo -n -e "\t"

        j=0
        l=0
        # add keyword usages
        while (($j < n)); do
            echo -e -n "${option[$j]}"
            # add its length on total length (number of charaters)
            sl=$(echo -n ${option[$j]} | wc -m)
            ((l=l+sl))
            k=$((n-1))
            # if it's not the last usage, put a comma
            if [ $j -lt $k ]; then
                echo -n ", "
                ((l=l+2))
            fi
            ((j=j+1))
        done
        # print spaces so every descriptions will be aligned
        repeat_spaces $((20-l))
        echo ${option[-1]}
    done
}

createClass()
{
    if [[ ! $1 =~ ^[a-zA-Z_]+$ ]]; then
        echo "Error : class name must only contains alphanumeric characters and underscores"
    else
        echo "Creating class $1..."
        if [ ! -d $srcDir ]; then
            echo -e "Project's structure not created yet.\nAborting."
            getHelp
            exit 2
        fi

        if [ $projectMode -eq 0 ]; then
            srcPath="$srcDir/$1.$fileExt"
            incPath="$incDir/$1.h"
        else
            folderPath="$srcDir/$1"
            mkdir -p $folderPath
            srcPath="$folderPath/$1.$fileExt"
            incPath="$folderPath/$1.h"
        fi

        if [ -f "$srcPath" ] || [ -f "$incPath" ]; then
            echo "A file with this name already exists."
            echo "Aborting"
        else
            touch $srcPath
            touch $incPath
            getSrcCode $1 > $srcPath
            getHeaderCode $1 > $incPath
            echo "Done"
        fi
    fi
}

createBaseProject() {
    echo "Creating structure..."
    i=0
    if [ ! -d "$srcDir" ]; then
        mkdir $srcDir
        [ $verbose -eq 1 -a $? -eq 0 ] && echo "Created $srcDir folder"
        ((i=i+1))
    fi

    if [ $projectMode -eq 0 ] && [ ! -d "$incDir" ]; then
        mkdir $incDir
        [ $verbose -eq 1 -a $? -eq 0 ] && echo "Created $incDir folder"
        ((i=i+1))
    fi

    if [ ! -d "$binDir" ]; then
        mkdir $binDir
        [ $verbose -eq 1 -a $? -eq 0 ] && echo "Created $binDir folder"
        ((i=i+1))
    fi

    if [ ! -d "$buildDir" ]; then
        mkdir $buildDir
        [ $verbose -eq 1 -a $? -eq 0 ] && echo "Created $buildDir folder"
        ((i=i+1))
    fi
    
    if [ ! -f "$srcDir/main.$fileExt" ]; then
        touch "$srcDir/main.$fileExt"
        getMainCode > "$srcDir/main.$fileExt"
        [ $verbose -eq 1 -a $? -eq 0 ] && echo "Created main file"
        ((i=i+1))
    fi

    if [ ! -f "Makefile" ]; then
        touch "Makefile"
        getMakefileCode > "Makefile"
        [ $verbose -eq 1 -a $? -eq 0 ] && echo "Created Makefile"
        ((i=i+1))
    fi

    [ $verbose -eq 1 -a $i -eq 0 ] && echo "No changes made" || echo "Done"
}

getSrcCode()
{
    if [ $fileExt == "c" ]; then
        echo -e -n "#include \"$1.h\"\n\n"
    else
        echo -e -n "#include \"$1.h\"\n\n$1::$1() {\n\n}\n\n$1::~$1() {\n\n}"
    fi
}

getHeaderCode()
{
    if [ $fileExt == "c" ]; then
        echo -e -n "#ifndef ${1^^}_H\n#define ${1^^}_H\n\n\n#endif // ${1^^}_H"
    else
        echo -e -n "#ifndef ${1^^}_H\n#define ${1^^}_H\n\nclass $1 {\n\tpublic:\n\t\t$1();\n\t\t~$1();\n};\n\n#endif // ${1^^}_H"
    fi
}

getMainCode() {
    if [ $fileExt == "cpp" ]; then
        echo -e -n "#include <iostream>\n\nint main(int argc, char **argv) {\n\tstd::cout << \"Hello World\" << std::endl;\n\treturn 0;\n}"
    elif [ $fileExt == "c" ]; then
        echo -e -n "#include <stdio.h>\n\nint main(int argc, char **argv) {\n\tprintf(\"Hello World \");\n\treturn 0;\n}"
    fi
}


setMode()
{
    [ $1 -eq 0 -o $1 -eq 1 ] && sed -i -e "s/projectMode=[0-9]/projectMode=$1/g" $0 || echo "Unknown mode : $1"
}

moveRec()
{
    # recursion limit
    if [ $5 -gt 500 ]; then
        echo "Too much recursion. Stop. ($5)"
        return 3
    fi

    # empty folder
    if [ -z "$(ls -A $4)" ]; then
        [ $6 -eq 1 ] && echo -e "\033[0;90m (Empty)\033[0m"
        return 0
	elif [ $6 -eq 1 ]; then
		echo
    fi

    # for each files / subdirs
    for i in "$4"/*; do
        # verbose
        if [ $6 -eq 1 ]; then
            [ -f $i ] && s="\n" || s="/"
            repeat_spaces $(($5))
            echo -e -n "└ $(basename "$i")$s"
        fi
        #

        # file
        if [ -f $i ]; then
            ext=$(echo "$i" | sed 's/^.*\.//')
            # .h
            if [ $ext == 'h' ]; then
                # filename.ext
                suffix="$(basename "$i")"
                # left trim folders
                subDir=${i/#"$2/"}
                [ $suffix == $subDir ] && newFileDir="$3/" || newFileDir="$3/${subDir/%"/$suffix"}/"

                # make folders + move file
                mkdir -p "$newFileDir"
                mv "$i" "$newFileDir"

                if [ $? -eq 0 ]; then
                    # verbose
                    if [ $6 -eq 1 ]; then
                        repeat_spaces $5
                        echo -e "\033[0;90m   > moved in $newFileDir\033[0m"
                    fi
                else
                    echo -e "\033[0;31mFailed to move $i\033[0m"
                    return 4
                fi
                #
            fi

        # dir
        else
            moveRec $1 $2 $3 $i $(($5 + 1)) $6
        fi
    done

    return 0
}

swapMode()
{
    if [ $1 -eq 1 ]; then
        dir=$incDir
        dest=$srcDir
    else
        dir=$srcDir
        dest=$incDir
        if [ ! -d "$incDir" ]; then
            mkdir $incDir
            if [ $2 -eq 1 ]; then
                if [ $? -eq 0 ]; then
                    echo "Create $incDir/ folder"
                else
                    echo "Failed to swap: cannot create $incDir/ folder"
                    exit 2
                fi
            fi
        fi
    fi
    [ $2 -eq 1 ] && echo -e -n "└ $dir/"
    # moveRec newMode source dest currentHeadersDir stage verbose
    moveRec $1 $dir $dest $dir 1 $2
    res=$?

    if [ $1 -eq 1 -a $res -eq 0 ]; then
        rm -r $incDir
        [ $2 -eq 1 ] && echo "Delete $incDir/ folder"
    fi

    [ $res -eq 0 ] && echo -e "\033[0;32mSuccesfully swapped\033[0m" || echo -e "\033[0;31mFailed to swap\033[0m"
    setMode $1
}

compile()
{
    make ${mode^^}=1 PGNAME=$pgname SRCEXT=$fileExt SRCDIR=$srcDir INCDIR=$includeDir OUT=$binDir BUILDDIR=$buildDir MODE=$projectMode CVERSION=$cVersion CPPVERSION=$cppVersion OSX=$os
}

launch()
{
    if [ ! -d "$srcDir" ] || [ $projectMode -eq 0 -a ! -d "$incDir" ]; then
        echo -e "There's no project's structure.\nTo create it, write $0 -g"
        exit 1
    fi

    j=0
    verbose=0
	pseudoMode=$mode
    shouldRun=1

    for i in 1 2 3 4; do
        if [ $i -le $# ]; then
            # release / debug mode / program name
            if [ $# -gt 0 ] && [ ${!i} == "-d" -o ${!i} == "-r" -o ${!i} == "--debug" -o ${!i} == "--release" ]; then
				pseudoMode=$mode
                if [[ ${!i} == "-d" || ${!i} == "--debug" || ${!i} == "-dev" ]]; then
					mode="debug"
					[ ${!i} == "-dev" ] && pseudoMode="dev" || pseudoMode="debug"
				else
					mode="release"
				fi
                ((j=j+1))
            fi

            if [ ${!i} == "-f" -o ${!i} == "--force" ]; then
                make clean
                ((j=j+1))
            fi

            if [ ${!i} == "-v" -o ${!i} == "--verbose" ]; then
                verbose=1
                ((j=j+1))
            fi

            if [ ${!i} == "--no-run" ]; then
                shouldRun=0
            fi
        fi
    done

	[ $mode == "dev" ] && mode="debug"

    while ((j > 0)); do
        shift
        ((j=j-1))
    done

    # detect os and adapt executable's extension
    if [ "$os" == "mac" ]; then # mac OS
        pgname=$pgname.app
    elif [ "$os" == "windows" ]; then # windows
        pgname=$pgname.exe
    fi

    # get header files folder
    [[ $projectMode -eq 1 ]] && includeDir=$srcDir || includeDir=$incDir

    if [ $verbose -eq 1 ]; then
        echo -e "\033[0;35mCompiling..."
    fi

	echo -e -n "\033[0;90m"

    # compile and execute if succeed
    [ $verbose -eq 1 ] && compile || compile 2> /dev/null

    res=$?

	echo -e "\033[0m"

    if [ $res -eq 0 ]; then
        cp $binDir/$mode/$pgname $outDir/$pgname

        if [ $verbose -eq 1 ]; then
            echo -e "\n\033[0;32mCompilation succeed\033[0m\n"
            echo -e "----- Executing ${mode^^} mode -----\n\n"
        fi
        
        if [ $shouldRun -eq 1 ]; then
            cd $outDir/
            if [ $pseudoMode == "debug" ]; then
                gdb ./$pgname $@
            else
                ./$pgname $@
            fi
        fi
    else
        echo -e "\n\033[0;31mCompilation failed\033[0m\n"
    fi

    echo -e "\033[0m"
}


patch()
{
    wget -q -O "$0" "$updateUrl"
    version=${version:-0.1}
    if [ $? -eq 0 ]; then
        newVersion="$(echo $(grep version=[0-9]\.[0-9] $0) | sed -r 's/version=//')"
        
        if [ $version == $newVersion ]; then
            echo "Already on latest version ($version)."
        else
            echo "v$version -> v$newVersion"
            echo -e "\033[0;32mSuccessfully updated\033[0m"
        fi

        # save our current mode, don't take the online one.
        setMode $projectMode
    else
        echo -e "\033[0;31mFailed to update\033[0m"
    fi
    exit 0
}

############

if [ $# -gt 0 ]; then
    # create class
    if [ $1 == "-c" ]; then
        [ $# -gt 1 ] && createClass $2 || echo "Error : no class name provided."

    # generate base project
    elif [ $1 == "-g" -o $1 == "--generate" ]; then
        [ $# -gt 1 ] && [ $2 == "-v" -o $2 == "--verbose" ]  && verbose=1 || verbose=0
        createBaseProject $verbose

    # help
    elif [ $1 == "-h" -o $1 == "--help" ]; then
        getHelp

    # patch
    elif [ $1 == "--patch" ]; then
        patch

    # version
    elif [ $1 == "-V" -o $1 == "--version" ]; then
        echo $version

    # swap mode
    elif [ $1 == "--swap-mode" ]; then
        [ $# -gt 1 ] && [ $2 == "-v" -o $2 == "--verbose" ]  && verbose=1 || verbose=0
        swapMode $((1 - $projectMode)) $verbose

    else
        launch $@
    fi
else
    launch $@
fi