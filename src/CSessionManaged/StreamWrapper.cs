using System;
using System.Runtime.InteropServices.ComTypes;

namespace ispsession.io
{
  //  internal sealed unsafe class StreamWrapper: Interfaces.IStream
  //  {
       
  //      private long m_iWritePos;    // Current index position for writing to the buffer.

  //      private long m_ulSize;        // length of the buffer
  //      private long m_ulLength;     // Total assigned buffer length.
  //                                   //ULONG		m_refCount;
  //                                   //bool		m_DidAlloc;
  //private enum STREAM_SEEK: uint
  //      {
  //          STREAM_SEEK_SET = 0u,
  //          STREAM_SEEK_CUR = 1u,
  //          STREAM_SEEK_END = 2u
  //      }
       
  //      public StreamWrapper()
  //      {

  //      }
  //      public StreamWrapper(byte[] buffer)
  //      {
  //          m_buf = buffer;
  //          m_ulLength =
  //          m_ulSize = buffer.Length;

  //      }
      

  //      private byte[] m_buf;
  //      private const int m_align32 = 32;
  //      private static uint MEMALIGN_32(uint cb)
  //      {
  //          return cb % m_align32 != 0 ? ((cb / m_align32) + 1) * m_align32 : cb;
  //      }
  //      public uint Read(byte[] pv, uint cb, uint* pcbRead)
  //      {
  //          // Check parameters.
  //          if (pcbRead != null)
  //          {
  //              pcbRead = (uint*)0;
  //          }
  //          if (pv == null)
  //          {
  //              return NativeMethods.E_INVALIDARG;
  //          }
  //          if (cb == 0)
  //          {
  //              return NativeMethods.STG_INVALID_POINTER;
  //          }
  //          // Calculate bytes left and bytes to read.
  //          var cBytesLeft = m_ulSize - m_iWritePos;
  //          var cBytesRead = cb > cBytesLeft ? cBytesLeft : cb;

  //          // If no more bytes to retrieve return S_FALSE.
  //          if (cBytesLeft == 0)
  //          {
  //              return NativeMethods.S_FALSE;
  //          }
  //          // Copy to users buffer the number of bytes requested or remaining
  //          Buffer.BlockCopy(m_buf, (int)m_iWritePos, pv, 0, (int)cBytesRead);
          
  //          m_iWritePos += cBytesRead;

  //          // Return bytes read to caller.
  //          if (pcbRead != null)
  //          {
  //              *pcbRead = (uint)cBytesRead;
  //          }

  //          return cb != cBytesRead ? NativeMethods.S_FALSE : NativeMethods.S_OK;
  //      }

  //      public uint Write(byte[] pv, uint cb, uint* pcbWritten)
  //      {
  //          if (pv == null)
  //          {
  //              return NativeMethods.STG_E_INVALIDPOINTER;
  //          }
  //          if (pcbWritten != null)
  //          {
  //              *pcbWritten = 0;
  //          }
  //          if (cb == 0) return NativeMethods.S_OK;

  //          if (m_iWritePos  + cb > m_ulSize )
  //          {

  //              m_ulSize  += ((m_iWritePos  + cb) - m_ulSize );
  //          }

  //          // Enlarge the current buffer.
  //          // eg: 9 + 4bytes > 10 (=13) m_ulLength
  //          if (m_iWritePos  + cb > m_ulLength )
  //          {
  //              m_ulLength  += ((m_iWritePos  + cb) - m_ulLength );
  //              m_ulLength  = MEMALIGN_32((uint)m_ulLength );
  //              // Grow internal buffer to new size.
  //              Array.Resize(ref m_buf, (int)m_ulLength);
  //          }
  //          // Copy callers memory to internal bufffer and update write position.
  //          Buffer.BlockCopy(pv, 0, m_buf, (int)m_iWritePos, (int)cb);
  //          m_iWritePos  += cb;
  //          // Return bytes written to caller.
  //          if (pcbWritten != null)
  //          {
  //              *pcbWritten = cb;
  //          }

  //          return NativeMethods.S_OK;
  //      }

  //      public uint Seek(long dlibMove, uint dwOrigin,  long* plibNewPosition)
  //      {

  //          //NOTE: we don't bother to write 2G limit memory for our purposes! 
  //          // Therefore, the HighPart of dbLibMove is ignored
  //          switch ((STREAM_SEEK)dwOrigin)
  //          {
  //              case STREAM_SEEK.STREAM_SEEK_SET:
  //                  m_iWritePos = dlibMove;
  //                  break;
  //              case STREAM_SEEK.STREAM_SEEK_CUR:
  //                  m_iWritePos += dlibMove;
  //                  break;
  //              case STREAM_SEEK.STREAM_SEEK_END:
  //                  m_iWritePos = m_ulSize;
  //                  break;
  //              default:
  //                  return NativeMethods.STG_E_INVALIDFUNCTION;
                  
  //          }
  //          if (dwOrigin != (uint)STREAM_SEEK.STREAM_SEEK_END)
  //          {
  //              if (m_iWritePos > m_ulLength)
  //              {
  //                  m_ulLength = m_ulSize;
  //                  m_ulLength = MEMALIGN_32((uint)m_iWritePos);
  //                  Array.Resize(ref m_buf, (int)m_ulLength);
  //              }
  //          }
  //          if (plibNewPosition != null)
  //          {
  //              *plibNewPosition = m_iWritePos;
  //          }
  //          return NativeMethods.S_OK;
  //      }

  //      public uint SetSize(long libNewSize)
  //      {
  //          long li;
  //          li = libNewSize;
  //          m_ulLength = MEMALIGN_32((uint)li);
  //          Array.Resize(ref m_buf, (int)m_ulLength);
          
  //          m_ulSize = li;
  //          if (m_iWritePos > li)
  //          {
  //              m_iWritePos = li;
  //          }
  //          return NativeMethods.S_OK;

  //      }

  //      public uint CopyTo(Interfaces.IStream pstm, long cb, long* pcbRead, long* pcbWritten)
  //      {
  //          uint hr = NativeMethods.S_OK;
  //          var tmpBuffer = new byte[512];
  //          uint bytesRead, bytesWritten, copySize;
  //          long totalBytesRead;
  //          long totalBytesWritten;
  //          if (pstm == null)
  //              return NativeMethods.STG_E_INVALIDPOINTER;

  //          totalBytesRead = 0;
  //          totalBytesWritten = 0;
  //          var backupWRitePos = m_iWritePos;
  //          m_iWritePos = 0;

  //          while (cb > 0 && hr == NativeMethods.S_OK)
  //          {

  //              copySize = Math.Min((uint)cb, 512);
  //              hr = Read(tmpBuffer, copySize, &bytesRead);
  //              if (hr < 0)
  //                  break;

  //              totalBytesRead += bytesRead;

  //              if (bytesRead> 0)
  //              {
  //                  hr = pstm.Write(tmpBuffer, bytesRead, &bytesWritten);
  //                  if (hr < 0)
  //                      break;

  //                  totalBytesWritten += bytesWritten;
  //              }

  //              if (bytesRead != copySize)
  //                  cb = 0;
  //              else
  //                  cb -= bytesRead;
  //          }
  //          m_iWritePos = backupWRitePos;

  //          if (pcbRead != null) *pcbRead = totalBytesRead;
  //          if (pcbWritten != null) *pcbWritten = totalBytesWritten;

  //          return hr;
  //      }

  //      public uint Commit(uint grfCommitFlags)
  //      {
  //          return NativeMethods.E_NOTIMPL;
  //      }

  //      public uint Revert()
  //      {
  //          return NativeMethods.E_NOTIMPL;
  //      }

  //      public uint LockRegion(long libOffset, long cb, uint dwLockType)
  //      {
  //          return NativeMethods.E_NOTIMPL;
  //      }

  //      public uint UnlockRegion(long libOffset, long cb, uint dwLockType)
  //      {
  //          return NativeMethods.E_NOTIMPL;
  //      }

  //      public uint Stat( out STATSTG pstatstg, uint grfStatFlag)
  //      {
  //          pstatstg = new STATSTG
  //          {
  //              cbSize = m_ulSize
  //          };
  //          return NativeMethods.S_OK;
  //      }

  //      public uint Clone(out Interfaces.IStream ppstm)
  //      {
  //          ppstm = null;
  //          return NativeMethods.E_NOTIMPL;
  //      }
  //  }
}
