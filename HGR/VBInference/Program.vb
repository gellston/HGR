Imports System
Imports OpenCvSharp

Module Program
    Sub Main(args As String())
        Try
            Dim memoryPool = HGRAPI.V1.MemoryPool.Create()
            Dim hgr = HGRAPI.V1.HGR.Create()

            hgr.Setup(HGRAPI.V1.DLType.Ghost3D, HGRAPI.V1.Device.Cuda)
            hgr.EmaAlpha = 0.2F

            Dim sampler = HGRAPI.V1.ClipSampler.Create()
            sampler.MaxFrames = 40
            sampler.SampleFrames = 16

            Using cap As New VideoCapture(0)
                If Not cap.IsOpened() Then
                    Console.WriteLine("Failed to open VideoCapture (camera 0).")
                    Return
                End If

                Using frame As New Mat()
                    While True
                        If Not cap.Read(frame) OrElse frame.Empty() Then
                            Console.WriteLine("Failed to read frame or empty frame.")
                            Exit While
                        End If

                        Using dlImage = HGRAPI.V1.Image.Create(CUInt(frame.Cols), CUInt(frame.Rows), 3, frame.Data, memoryPool)
                            Using resizeImage = HGRAPI.V1.Image.Resize(dlImage, 128, 64)
                                sampler.Append(resizeImage)
                            End Using
                        End Using

                        Dim samples = sampler.RequestSampling()
                        Dim result = hgr.Predict(samples)

                        Cv2.ImShow("capture", frame)

                        Dim key As Integer = Cv2.WaitKey(1)
                        If key = 27 Then Exit While

                        Console.WriteLine("name : {0} prob : {1}", result.Name, result.Prob)

                        HGRAPI.V1.ClipSampler.DisposeImages(samples)
                    End While
                End Using

                cap.Release()
                Cv2.DestroyAllWindows()
            End Using

        Catch ex As Exception
            Console.WriteLine($"Exception: {ex}")
        End Try
    End Sub
End Module
