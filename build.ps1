if (Test-Path -Path .\build)
{
    rm -R -Fo  .\build
    mkdir build | OUT-NULL
}
Push-Location .\build
try
{
    cmake ..
    cmake --build . --config Release -v
}
finally
{
    Pop-Location
}