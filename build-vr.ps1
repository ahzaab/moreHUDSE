if (Test-Path -Path .\build)
{
    rm -R -Fo  .\build
    mkdir build | OUT-NULL
}
Push-Location .\build
try
{
    cmake -DUSE_VR_BUILD=ON ..
    cmake --build . --config Release -v
}
finally
{
    Pop-Location
}